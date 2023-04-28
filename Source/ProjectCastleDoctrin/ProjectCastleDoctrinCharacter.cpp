// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectCastleDoctrinCharacter.h"
#include "ProjectCastleDoctrinProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AProjectCastleDoctrinCharacter

AProjectCastleDoctrinCharacter::AProjectCastleDoctrinCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AProjectCastleDoctrinCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AProjectCastleDoctrinCharacter::OnCollisionBeginOverlap);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AProjectCastleDoctrinCharacter::OnCollisionEndOverlap);
	}

	// Ensure that the movement speed starts with walking
	GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeed;

	// Ensure that stamina is the max
	_stamina = _maxStamina;

	// Setup Delegate Bindings for Timers

	// Function to allow gun to shoot again
	_inBetweenShotsDelegate.BindUFunction(this, "resetShoot");

	_regenDelegate.BindUFunction(this, "healthRegen");

	// Start timer until player regens health
	_healthRegenDelegate.BindUFunction(this, "startHealthRegen");

	// Start timer to prevent damage for a time
	_damagedDelegate.BindUFunction(this, "resetCanBeDamaged");

	// Start timer to recover stamina
	_staminaRecoveryDelegate.BindUFunction(this, "recoverStamina");
}

//////////////////////////////////////////////////////////////////////////// Input

void AProjectCastleDoctrinCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::Look);

		//Shooting should be handled when a gun is set or switched in
		//EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::Shoot);

		//Changing Weapon
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::ChangeWeapon);

		//Reloading Weapon
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::ReloadWeapon);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AProjectCastleDoctrinCharacter::Interact);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AProjectCastleDoctrinCharacter::SprintDone);

		EnhancedInputComponent->BindAction(ShowScoreboardAction, ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::ShowScoreboard);
		EnhancedInputComponent->BindAction(ShowScoreboardAction, ETriggerEvent::Completed, this, &AProjectCastleDoctrinCharacter::HideScoreboard);
	}
}

void AProjectCastleDoctrinCharacter::ShowScoreboard(const FInputActionValue& Value)
{
	DoShowScoreboard();
}

void AProjectCastleDoctrinCharacter::HideScoreboard(const FInputActionValue& Value)
{
	DoHideScoreboard();
}

void AProjectCastleDoctrinCharacter::SprintDone(const FInputActionValue& Value)
{
	// Check if the stamina recovery is already happening
	if (!_staminaRecoveryTimer.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sprint Done"));

		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeed;

		// Call to update UI to not show sprinting
		StopSprintUIUpdate();

		GetWorld()->GetTimerManager().SetTimer(_staminaRecoveryTimer, _staminaRecoveryDelegate, _staminaRecoveryTime, true);
	}
}

void AProjectCastleDoctrinCharacter::Sprint(const FInputActionValue& Value)
{
	// This could decrement very fast depending on how fast this ongoing event type fires, might need to use a timer instead
	// A timer would work but the difficulty would be holding down vs tapping for sprinting
	// Determine if we can sprint or not
	if (_stamina > 0.0)
	{
		GetWorld()->GetTimerManager().ClearTimer(_staminaRecoveryTimer);

		GetCharacterMovement()->MaxWalkSpeed = _maxSprintSpeed;

		float newStamina = _stamina - _staminaDecrementAmnt;
		UE_LOG(LogTemp, Warning, TEXT("Sprinting stamina at: %f"), newStamina);
		if (newStamina < 0)
		{
			_stamina = 0;
			// Force "release" of the button?

			GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeed;

			StopSprintUIUpdate();

			// Call to update UI to not show sprinting

			// Start stamina recovery timer
			GetWorld()->GetTimerManager().SetTimer(_staminaRecoveryTimer, _staminaRecoveryDelegate, _staminaRecoveryTime, true);
		}
		else
		{
			SprintUIUpdate();

			_stamina = newStamina;
		}
	}
}

void AProjectCastleDoctrinCharacter::recoverStamina()
{
	UE_LOG(LogTemp, Warning, TEXT("Recovering Stamina"));
	_stamina += _staminaIncreaseAmnt;
	if (_stamina > _maxStamina)
	{
		_stamina = _maxStamina;
		GetWorld()->GetTimerManager().ClearTimer(_staminaRecoveryTimer);
	}
}

void AProjectCastleDoctrinCharacter::Interact(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("interact"));
	// This would change to deal with any interactable which would require some subclassing
	if (_interactable)
	{
		UE_LOG(LogTemp, Warning, TEXT("interact has valid interactable object"));
		// If weapon is available to be taken, aka the station was used and a weapon is present don't allow another roll of the station until it's gone
		// Usable would be an overriden function to allow for other behavior of other interactables
		if (_interactable->usable())
		{
			UE_LOG(LogTemp, Warning, TEXT("interactable is usable"));
			// This is specific to BuyStation
			if (_points >= _interactable->getCost())
			{
				UE_LOG(LogTemp, Warning, TEXT("player has enough points"));

				// Remove _points
				_points -= _interactable->getCost();

				_interactable->interact();
			}
		}
		else
		{
			// Cast to the box
			ABuyStation* station = Cast<ABuyStation>(_interactable);
			// Take the weapon
			if (station)
			{
				ABoxSpawnedWeapon* weapon = station->getSpawnedWeapon();
				if (weapon)
				{
					station->cancelGunTimer();
					createGun(weapon->getGunData());
					station->destroySpawnedWeapon();
				}
			}

		}

	}
}

void AProjectCastleDoctrinCharacter::ReloadWeapon(const FInputActionValue& Value)
{
	// Start reload timer
	GetWorld()->GetTimerManager().SetTimer(_reloadTimer, _reloadDelegate, _gunInUse->getReloadTime(), false);
	//_gunInUse->reload();
}

void AProjectCastleDoctrinCharacter::ChangeWeapon(const FInputActionValue& Value)
{
	int index = _guns.Find(_gunInUse);
	int ogIndex = index;

	UE_LOG(LogTemp, Warning, TEXT("magnitude: %f"), Value.GetMagnitude());

	if (_guns.Num() > 1)
	{
		// Should never be below zero
		// If index is 0 then we should only allow scrolling up
		if (index <= 0)
		{
			if (Value.GetMagnitude() == 1)
			{
				index++;
			}
		}
		// If index is max guns then only allow scrolling down
		else if (index >= _guns.Num() - 1)
		{
			if (Value.GetMagnitude() == -1)
			{
				index--;
			}
		}
		else
		{
			if (Value.GetMagnitude() == 1)
			{
				index++;
			}
			else if (Value.GetMagnitude() == -1)
			{
				index--;
			}
		}

		if (index != ogIndex)
		{
			UE_LOG(LogTemp, Warning, TEXT("change weapon index: %i"), index);

			// Play Weapon Switch Sound
			if (weaponSwitchSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), weaponSwitchSound, GetActorLocation());
			}

			// Unbind Action
			UInputComponent* inputComponent = InputComponent;
			if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(inputComponent))
			{
				// Shoot binding should always be set so no checking should be necessary
				EnhancedInputComponent->RemoveBindingByHandle(shootBinding);
			}

			_reloadDelegate.Unbind();

			_gunInUse = _guns[index];

			// Have to bind here because _gunInUse changes but since it is just a base gun we might be able to move it up
			_reloadDelegate.BindUFunction(_gunInUse, "reload");

			// Need the original location of the socket to properly reposition the new gun
			FVector origLoc = Mesh1P->GetSocketLocation(FName("Grip"));

			updateGunMesh();

			// Reposition gun
			FVector gripLoc = Mesh1P->GetSocketLocation(FName("Grip"));
			Mesh1P->SetWorldLocation(Mesh1P->GetComponentLocation() + (origLoc - gripLoc));

			// Change Shoot Binding for auto to semi and reverse
			setupShootBinding();
		}
	}
}

void AProjectCastleDoctrinCharacter::Shoot(const FInputActionValue& Value)
{
	// Get Gun Component and call shoot
	if (_gunInUse && _canShoot)
	{
		// Pass in camera vector so that we can shoot at angles
		//_gunInUse->Shoot(Mesh1P, GetActorForwardVector(), this);
		if (_gunInUse->Shoot(Mesh1P, FirstPersonCameraComponent->GetForwardVector(), this))
		{
			_points += 100;

			// Tell HUD hit occured
			enemyHit();
		}

		_canShoot = false;

		// Start timer until the gun can be shot again
		GetWorld()->GetTimerManager().SetTimer(_inBetweenShotsTimer, _inBetweenShotsDelegate, _gunInUse->getTimeInBetweenShots(), false);
	}
}

void AProjectCastleDoctrinCharacter::resetShoot()
{
	_canShoot = true;
}

void AProjectCastleDoctrinCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AProjectCastleDoctrinCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProjectCastleDoctrinCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AProjectCastleDoctrinCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AProjectCastleDoctrinCharacter::addGunToInventory(UBaseGun* gun)
{
	if (gun)
	{
		_guns.Add(gun);

		if (_guns.Num() == 1)
		{
			_gunInUse = gun;

			// Same thing here as shoot
			_reloadDelegate.BindUFunction(_gunInUse, "reload");

			// Update the Mesh
			updateGunMesh();
		}
	}
}

void AProjectCastleDoctrinCharacter::updateGunMesh()
{
	if (_gunInUse->getGunMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("gun mesh is valid"));
		Mesh1P->SetSkeletalMesh(_gunInUse->getGunMesh());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("gun mesh isnt valid"));
	}
}

void AProjectCastleDoctrinCharacter::applyDamage(float damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Player damaged"));

	// Play Player Hit sound
	if (playerHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), playerHitSound, GetActorLocation());
	}

	_health -= damage;

	// Stop health regen timer
	GetWorld()->GetTimerManager().ClearTimer(_healthRegenTimer);
	_healthRegenTimer.Invalidate();
	GetWorld()->GetTimerManager().ClearTimer(_regenTimer);
	_regenTimer.Invalidate();

	playerHit();

	if (_health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player died------------------"));
		// End the game
		gameEnded();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(_healthRegenTimer, _healthRegenDelegate, _timeUntilHealthRegen, false);
	}
}

void AProjectCastleDoctrinCharacter::startHealthRegen()
{
	// Actual health regen timer
	GetWorld()->GetTimerManager().SetTimer(_regenTimer, _regenDelegate, _timeInBetweenRegen, true);
}

void AProjectCastleDoctrinCharacter::healthRegen()
{
	if (_health + _regenHealthAmnt >= _maxHealth)
	{
		_health = _maxHealth;
		healthRegenUIUpdate();
		
		GetWorld()->GetTimerManager().ClearTimer(_regenTimer);
		_regenTimer.Invalidate();
	}
	else
	{
		_health += _regenHealthAmnt;
		healthRegenUIUpdate();
	}
}

void AProjectCastleDoctrinCharacter::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("collision begin overlap called"));
	AInteractableParent* interactable = Cast<AInteractableParent>(OtherActor);
	if (interactable)
	{
		// Overlapped with Interactable
		// Allow buying
		_interactable = interactable;
		showBuyStationCost(_interactable->getCost());
	}

	/*ABuyStation* buyStation = Cast<ABuyStation>(OtherActor);

	if (buyStation)
	{
		UE_LOG(LogTemp, Warning, TEXT("collision with buy station"));
		_interactable = buyStation;

		showBuyStationCost(_interactable->getCost());
	}*/

	// Test if OtherActor is ammo
	AAmmoItem* ammo = Cast<AAmmoItem>(OtherActor);

	if (ammo)
	{
		_gunInUse->ammoReceived(ammo->getAmntOfAmmo());

		if (GetWorld())
		{
			GetWorld()->DestroyActor(ammo);
		}
	}

	// Test if OtherActor is enemy (Cockroach)
	ACockroach* enemy = Cast<ACockroach>(OtherActor);

	if (enemy && _canBeDamaged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Overlapped by cockroach"));
		applyDamage(enemy->damageDealt());

		GetWorld()->GetTimerManager().SetTimer(_damagedTimer, _damagedDelegate, _timeUntilDamageCanOccur, false);

		_canBeDamaged = false;
	}
}

void AProjectCastleDoctrinCharacter::OnCollisionEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("collision ended"));
	AInteractableParent* interactable = Cast<AInteractableParent>(OtherActor);

	if (interactable)
	{
		UE_LOG(LogTemp, Warning, TEXT("collision ended with buy station"));
		
		_interactable = nullptr;

		// This hides all costs
		hideBuyStationCost();
	}
}

float AProjectCastleDoctrinCharacter::getHealth()
{
	return _health;
}

float AProjectCastleDoctrinCharacter::getMaxHealth()
{
	return _maxHealth;
}

void AProjectCastleDoctrinCharacter::enemyKilled()
{
	_kills++;
}

void AProjectCastleDoctrinCharacter::resetCanBeDamaged()
{
	_canBeDamaged = true;
}

void AProjectCastleDoctrinCharacter::setupShootBinding()
{
	UInputComponent* inputComponent = InputComponent;
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(inputComponent))
	{
		if (_gunInUse && _gunInUse->getShootAction())
		{
			UE_LOG(LogTemp, Warning, TEXT("setting shooting binding up for: %s"), *_gunInUse->getName());
			
			// Might need to remove binding on switch, before it happens
			shootBinding = EnhancedInputComponent->BindAction(_gunInUse->getShootAction(), ETriggerEvent::Triggered, this, &AProjectCastleDoctrinCharacter::Shoot).GetHandle();
		}
	}
}

// TODO: Take into account the health of the enemies to determine if the user has enough ammo
bool AProjectCastleDoctrinCharacter::hasEnoughAmmo()
{
	for (auto it : _guns)
	{
		// if there is more than or equal to a full magazine in the magazine and or in reserve return
		if (it->getAmmoInMag() + it->getAmmoInReserve() >= it->getMagSize())
		{
			return true;
		}
	}

	return false;
}