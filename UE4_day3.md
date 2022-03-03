# UE4 (DAY-3)

- 캐릭터의 제작과 컨트롤

  - 인간형 폰 보다 효과적으로 제작하기 위해 캐릭터를 사용.
  - Chaacter 클래스를 생성하면 ACharacter 클래스를 상속받는다
    - ACharacter은 APawn 클래스를 상속받는다.
    - ACharacter 클래스는 Capsule 컴포넌트, SkeletalMesh 컴포넌트를 사용한다.
    - CharacterMovement 컴포넌트를 통해 움직임을 관리한다.
      - private로 선언된 컴포넌트의 포인터를 상속받은 클래스들이 접근할 수 있도록 GetCapsuleComponent, GetMesh, GetCharacterMovement라는 함수를 제공하고 있다.

  ```c++
  //생성자
  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));

  //캡슐 컴포넌트에 붙이기
  SpringArm->SetupAttachment(GetCapsuleComponent());

  //위치와 회전 설정
  GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

  //거리 설정
  SpringArm->TargetArmLength = 400.0f;
  //회전 설정
  SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

  //에셋 조회
  static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

  //에셋 조회 성공 시
  if (SK_CARDBOARD.Succeeded())
  {
  	GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
  }

  GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

  static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBluprint.WarriorAnimBluprint_C"));
  if (WARRIOR_ANIM.Succeeded())
  {
  	GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
  }

  //조작 키 설정
  void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
  {
  	Super::SetupPlayerInputComponent(PlayerInputComponent);

  	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
  	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
  }

  void AABCharacter::UpDown(float NewAxisValue)
  {
  	AddMovementInput(GetActorForwardVector(), NewAxisValue);
  }

  void AABCharacter::LeftRight(float NewAxisValue)
  {
  	AddMovementInput(GetActorRightVector(), NewAxisValue);
  }
  ```

  - 캐릭터 무브먼트 컴포넌트가 FloatingPawnMovement에 비해 가지는 장점

    1. 점프와 같은 중력을 반영한 움직임을 제공한다.
    2. 다양한 움직임을 설정할 수 있다. 걷기, 기어가기, 날아가기, 수영하기 등 다양한 이동 모드를 설정할 수 있음.
    3. 멀티 플레이 네트워크 환경에서 캐릭터들의 움직임을 자동으로 동기화한다.

  - 컨트롤 회전의 활용

    - 플레이어가 게임에 입장할 때 부여받는 두 종류의 액터로 플레이어 컨트롤러와 폰이 있다.

    - 폰이 관리하는 속성 중 대표적인 것은 속도임.

    - 반면 플레이어 컨트롤러는 플레이어의 의지를 나태내는 컨트롤 회전이라는 속성을 제공함

    - 마우스 감도에 따라 입력 값으로 대략적 -3부터 3까지의 작은 숫자들이 들어오는데, 해당 입력 값에 따라 캐릭터가 회전하도록 언리얼 엔진은 AddControllerInputYaw, Roll, Pitch라는 세 가지 명령을 제공한다.

      ```c++
      //ABCharacter.h
      void LookUp(float NewAxisValue);
      void Turn(float NewAxisValue);

      //ABCharacter.cpp
      void AABCharacter::LookUp(float NewAxisValue)
      {
      	AddControllerPitchInput(NewAxisValue);
      }

      void AABCharacter::Turn(float NewAxisValue)
      {
      	AddControllerYawInput(NewAxisValue);
      }
      ```

    - AddControllerYawInput과 AddControllerPitchInput 명령은 마우스 입력 신호 값을 플레이어 컨트롤러의 회전 값으로 변환하는 명령임.

    - 입력에 따라 변화되는 이 컨트롤 회전 값은 캐릭터의 카메라 설정에서 다양하게 사용된다. 언리얼 엔진의 캐릭터 모델은 기본으로 컨트롤 회전의 Yaw 회전(z축 회전) 값과 폰의 Yaw 회전이 연동되어 있다. 이를 지정하는 속성이 액터의 Pawn 섹션에 위치한 UseControllerRotationYaw다.

  - 3인칭 컨트롤 구현(GTA 방식)

    - 캐릭터의 이동 : 현재 보는 시점을 기준으로 상하, 좌우 방향으로 마네킹이 이동하고 카메라는 회전하지 않음

    - 캐릭터의 회전 : 캐릭터가 이동하는 방향으로 마네킹이 회전함

    - 카메라 지지대 길이 : 450cm

    - 카메라 회전 : 마우스 상하좌우 이동에 따라 카메라 지지대가 상하좌우로 회전

    - 카메라 줌 : 카메라 시선과 캐릭터 사이에 장애물이 감지되면 캐릭터가 보이도록 카메라를 장애물 앞으로 줌인

      SpringArm 컴포넌트, SetControlMode라는 멤버 함수 작성

      바라보는 방향 기준 앞뒤, 좌우 방향 벡터 값 가져옮

      ```c++
      //ABCharacter.cpp
      void AABCharacter::UpDown(float NewAxisValue)
      {
      	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
      }

      void AABCharacter::LeftRight(float NewAxisValue)
      {
      	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y),NewAxisValue);
      }
      ```

      캐릭터가 움직이는 방향으로 캐릭터를 자동으로 회전시켜주는 캐릭터 무브먼트 컴포넌트의 OrientRotationToMovement 기능을 사용한다.

      ```c++
      //ABCharacter.cpp
      void AABCharacter::SetControlMode(int32 ControlMode)
      {
      	if (ControlMode == 0)
      	{
      		SpringArm->TargetArmLength = 450.0f;
      		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
      		SpringArm->bUsePawnControlRotation = true;
      		SpringArm->bInheritPitch = true;
      		SpringArm->bInheritRoll = true;
      		SpringArm->bInheritYaw = true;
      		SpringArm->bDoCollisionTest = true;
      		bUseControllerRotationYaw = false;
      		GetCharacterMovement()->bOrientRotationToMovement = true;
      		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
      	}
      }
      ```

  - 3인칭 컨트롤 구현(디아블로 방식)

    - 캐릭터 이동 : 상하좌우 키를 조합해 캐릭터가 이동할 방향을 결정

    - 캐릭터의 회전 : 캐릭터는 입력한 방향으로 회전

    - 카메라 길이 : 조금 멀리 떨어진 800cm

    - 카메라 회전 : 카메라의 회전 없이 항상 고정 시선으로 45도로 내려봄

    - 카메라 줌 : 없음, 카메라와 캐릭터 간 장애물 잇을 시 외곽선으로 처리

      ​

    - 상하 키와 좌우 키를 조합해 캐릭터의 회전과 이동이 이뤄져야 함.

    - 이를 위해 각 축의 입력을 조합해 보관할 벡터 유형의 변수도 추가한다.

    - c++은 class 키워드로 열거형을 선언하는 방식과 class 없이 선언하는 방식, 두가지를 제공한다.

    ```c++
    enum class EControlMode
    	{
    		GTA,
    		DIABLO
    	};

    	void SetControlMode(EControlMode NewControlMode);
    	EControlMode CurrentControlMode = EControlMode::GTA;
    	FVector DirectionToMove = FVector::ZeroVector;
    ```

    - UPROPERTY를 사용하지 않는 FVector와 같은 값 타입 변수들은 항상 초기 값을 미리 지정하는 것이 안전함.
    - FRotationMatrix는 회전된 좌표계 정보를 저장하는 행렬, GTA 방식에서는 FRotator 값으로 회전 행렬을 생성하고, 이를 토대로 변환된 좌표계의 X, Y축 방향을 가져옴.
    - 디아블로 방식에서는 하나의 벡터 값과 이에 직교하는 나머지 두 축을 구해 행렬을 생성하고, 이와 일치하는 FRotator 값을 얻어오는 방식을 사용함. 하나의 벡터로부터 회전행렬을 구축하는 명령은 MakeFromX, Y, Z가 있는데, 두 축의 입력을 합산한 최종 벡터 방향과 캐릭터의 시선 방향(X축)이 일치해야 하므로 MakeFromX 사용함.

  - 컨트롤 설정의 변경

    ```c++
    //버튼 누르면 시점 변경
    PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
    ```

    - 시점 변환 시 카메라 길이가 부드럽게 전환되는 기능 추가
    - 캐릭터 컨트롤을 변경할 때마다 SpringArm의 길이와 회전 값이 목표 값까지 각각의 목표 설정 값으로 서서히 변경되도록 FMath 클래스에서 제공하는 InterpTo 명령어를 사용한다.
    - InterpTo 명령어는 지정한 속력으로 목표 지점까지 진행하되, 목표 지점까지 도달하면 그 값에서 멈춘다.
    - 컨트롤 회전을 gta 방식에서는 SpringArm의 회전에 사용하고, 디아블로 방식에서는 캐릭터 방향에 사용한다.

    ​

