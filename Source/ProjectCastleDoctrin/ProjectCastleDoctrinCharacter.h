// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Sound/SoundBase.h"

#include "AmmoItem.h"
#include "BaseGun.h"
#include "BuyStation.h"
#include "Cockroach.h"
#include "Door.h"

#include "ProjectCastleDoctrinCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
//class USoundBase;

UCLASS(config=Game)
class AProjectCastleDoctrinCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	// Using this for the gun
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	/** Change Weapon Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;
	
	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	/** Reload Interact Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Reload Interact Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShowScoreboardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float _maxSprintSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float _maxWalkSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// Percentage
	float _stamina = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// Percentage
	float _maxStamina = 1.0;

public:
	AProjectCastleDoctrinCharacter();

    // Get if player has enough ammo or put this logic in spawn manager
	bool hasEnoughAmmo();

protected:
	virtual void BeginPlay();

public:
		
	UPROPERTY(BlueprintReadOnly)
	UBaseGun* _gunInUse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	int _points = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	int _kills = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USoundBase* playerHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USoundBase* weaponSwitchSound;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	UFUNCTION(BlueprintCallable)
	void addGunToInventory(UBaseGun* gun);

	UFUNCTION(BlueprintImplementableEvent)
	void enemyHit();

	UFUNCTION(BlueprintImplementableEvent)
	void playerHit();

	UFUNCTION(BlueprintCallable)
	float getHealth();

	UFUNCTION(BlueprintCallable)
	float getMaxHealth();

	UFUNCTION(BlueprintImplementableEvent)
	void createGun(TSubclassOf<UBaseGun> gunClass);

	UFUNCTION(BlueprintImplementableEvent)
	void gameEnded();

	UFUNCTION()
	void enemyKilled();

	UFUNCTION(BlueprintCallable)
	void setupShootBinding();

	UFUNCTION(BlueprintImplementableEvent)
	void showBuyStationCost(int cost);

	UFUNCTION(BlueprintImplementableEvent)
	void hideBuyStationCost();

	UFUNCTION(BlueprintImplementableEvent)
	void healthRegenUIUpdate();

	UFUNCTION(BlueprintImplementableEvent)
	void DoShowScoreboard();

	UFUNCTION(BlueprintImplementableEvent)
	void DoHideScoreboard();

	UFUNCTION(BlueprintImplementableEvent)
	void SprintUIUpdate();

	UFUNCTION(BlueprintImplementableEvent)
	void StopSprintUIUpdate();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);

	void ChangeWeapon(const FInputActionValue& Value);

	void ReloadWeapon(const FInputActionValue& Value);

	void Interact(const FInputActionValue& Value);

	void Sprint(const FInputActionValue& Value);

	void SprintDone(const FInputActionValue& Value);

	void ShowScoreboard(const FInputActionValue& Value);

	void HideScoreboard(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:

	UFUNCTION()
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCollisionEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void updateGunMesh();

	void applyDamage(float damage);

	UFUNCTION()
	void resetShoot();

	UFUNCTION()
	void startHealthRegen();

	UFUNCTION()
	void resetCanBeDamaged();

	UFUNCTION()
	void healthRegen();

	UFUNCTION()
	void recoverStamina();

private:

	TArray<UBaseGun*> _guns;
	float _health = 100;
	float _maxHealth = 100;
	float _regenHealthAmnt = 2;
	AInteractableParent* _interactable = nullptr;

	bool _canShoot = true;

	FTimerHandle _inBetweenShotsTimer;
	FTimerDelegate _inBetweenShotsDelegate;
	FTimerHandle _damagedTimer;
	FTimerDelegate _damagedDelegate;
	FTimerHandle _healthRegenTimer;
	FTimerDelegate _healthRegenDelegate;
	FTimerHandle _regenTimer;
	FTimerDelegate _regenDelegate;
	FTimerHandle _reloadTimer;
	FTimerDelegate _reloadDelegate;
	FTimerHandle _staminaRecoveryTimer;
	FTimerDelegate _staminaRecoveryDelegate;

	uint32 shootBinding;

	bool _canBeDamaged = true;
	float _timeUntilDamageCanOccur = .75; // Seconds
	float _timeUntilHealthRegen = 3.0; // Seconds
	float _timeInBetweenRegen = .1; // Seconds

	// Decrements a percentage
	float _staminaDecrementAmnt = .1; // Seconds

	// Amnt of time in between recovery of stamina executions
	float _staminaRecoveryTime = .5; // Seconds

	float _staminaIncreaseAmnt = .1; // Seconds

};

