# UE4(day 2)

- 움직이는 액터 제작

  - 로깅 환경 설정

    - 출력 로그 윈도우

      ```c++
      UE_LOG(카테고리, 로깅 수준, 형식 문자열, 인자..)
      ```

  - 형식 문자열

    - C언어의 printf 함수와 같은 형식 문자열 기능을 지원함. 단, 문자열 사용 시 주의해야 한다. 문자열을 정의할 때는 모든 플랫폼에서 2바이트 문자열을 지원하는 TEXT 매크로를 사용하는 것이 좋다. 문자열을 관리하는 기본 클래스로 언리얼 엔진은 FString 클래스를 제공하는데, FString으로 선언된 변수에서 문자열 정보를 얻어오려면 반드시 * 연산자를 앞에 지정해줘야 한다.

      ```c++
      FString::Printf(TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
      ```

  - 로깅을 위한 공용 매크로 설정

    - 게임 로직에 대한 로그를 분류하기 위해 로그 카테고리를 직접 선언하면 유용하다.

    - 언리얼 엔진은 로그 카테고리를 선언하기 위해 두개의 매크로를 제공하는데, 하나는 선언부에 사용하고 하나는 구현부에 사용한다.

    - 게임 모듈명으로 된 헤더 파일과 소스 파일에 위의 매크로를 각각 선언하는 것이 일반적이다.

      ```c++
      //ArenaBattle.h
      #pragma once

      #include "EngineMinimal.h"

      DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);
      ```

      ```c++
      //ArenaBattle.cpp
      #include "ArenaBattle.h"
      #include "Modules/ModuleManager.h"

      DEFINE_LOG_CATEGORY(ArenaBattle);
      IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ArenaBattle, "ArenaBattle" );
      ```

      ```c++
      UE_LOG(ArenaBattle, Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
      ```

    - 매크로를 정의해 좀 더 간편하게 로그를 설정 가능

      - 로그를 남길 때 추가로 발생한 함수 이름과 코드 라인을 함께 출력

        - ABLOG_S : 코드가 들어있는 파일 이름과 함수, 라인 정보를 추가해 ArenaBattle 카테고리로 로그를 남김.
        - ABLOG : ABLOG_S 정보에 형식 문자열로 추가 정보를 지정해 로그를 남긴다.

        ```c++
        //ArenaBattle.h
        #pragma once

        #include "EngineMinimal.h"

        DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);
        #define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
        #define ABLOG_S(Verbosity) UE_LOG(ArenaBattle, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
        #define ABLOG(Verbosity, Format, ...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s %s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
        ```

  - 액터의 주요 이벤트 함수

    - 컴포넌트의 세팅이 완료돼 게임이라는 무대에 나설 수 있게 되면 언리얼 엔진의 액터의 PostInitializeComponents 함수를 호출한다.

    - 언리얼 엔진은 액터가 게임에 참여할 때 액터의 BeginPlay 함수를 호출하고, 매 프레임마다 액터의 Tick 함수를 호출한다.

    - 더 이상 액터의 역할이 없어지면, 액터는 게임에서 퇴장하고 메모리에서 소멸된다. 퇴장 시 EndPlay 호출.

    - 이런 것들이 이벤트 함수다.

      ```c++
      virtual void BeginPlay() override;
      virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
      virtual void PostInitializeComponents() override;
      ```

  - 움직이는 액터의 설계

    - Tick 함수

      - Tick 함수의 인자 DeltaSeconds : 이전 렌더링 프레임으로부터 현재 렌더링 프레임까지 소요된 시간을 알 수 있음.


      - 변수를 생성할 때 추가로 살펴봐야 할 부분은 데이터 은닉을 고려한 설계 방식이다.

        - 정보를 은닉하기 위해 해당 변수를 private로 선언하면 컴파일 과정에서 에러가 발생한다. 하지만 UPROPERTY 매크로에 AllowPrivateAccess라는 META 키워드를 추가하면 에디터에서는 이를 편집함과 동시에 변수 데이터를 은닉할 수 있게 돼 프로그래밍 영역에서 캡슐화가 가능해진다.

        ```c++
        private:
        	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
        		float RotateSpeed;
        ```

      - AddActorLocalRotation 함수는 회전을 할 수 있게 한다.

        - 회전에는 FRotator을 사용하며 Pitch, Yaw, Roll이라는 세 가지 회전 요소로 구성된다.
          - Pitch : 좌우를 기준으로 돌아가는 회전, Y축 회전
          - Yaw : 상하를 기준으로 돌아가는 회전, Z축 회전
          - Roll : 정면을 기준으로 돌아가는 회전, X축 회전

      - 언리얼 엔진에서 시간을 관리하는 주체는 월드임. 월드에는 시간 관리자(TimeManager)가 있고, 게임에 필요한 다양한 시간 값들을 얻어올 수 있다. Tick 함수 인자의 DeltaSeconds 값은 GetWorld()->GetDeltaSeconds() 함수를 사용해 가져올 수 있다.

        - 게임 시작 후 현재까지 경과된 시간: GetWorld()->GetTimeSeconds()
        - 사용자가 게임을 중지한 시간을 제외한 경과 시간: GetWorld()->GetUnpausedTimeSeconds()
        - 현실 세계의 경과 시간: GetWorld()->GetRealTimeSeconds()
        - 사용자가 게임을 중지한 시간을 제외한 현실 세계의 경과시간: GetWorld()->GetAudioTimeSeconds()

    - 무브먼트 컴포넌트의 활용

      - 무브먼트 컴포넌트 : 움직임이라는 요소를 분리해 액터와 별도로 관리하도록 프레임워크를 구성한 것

      - 액터는 무브먼트 컴포넌트가 제공하는 이동 메커니즘에 따라 움직인다. 즉, Tick 함수를 구현하지 않아도 동일하게 움직이는 액터를 제작할 수 있다.

        - FloatingPawnMovement : 중력의 영향을 받지 않는 액터의 움직임을 제공. 입력에 따라 자유롭게 움직임
        - RotatingMovement : 지정한 속도로 액터를 회전시킴
        - InterpMovement : 지정한 위치로 액터를 이동시킴
        - ProjectileMovement : 액터에 중력의 영향을 받아 포물선을 그리는 발사체의 움직임을 제공함. 주로 총알, 미사일 등에 사용

      - 무브먼트 컴포넌트를 이용한 회전

        ```c++
        //Fountain.h
        UPROPERTY(VisibleAnywhere)
          URotatingMovementComponent* Movement;

        //Fountain.cpp
        AFountain::AFountain()
        {
        	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));
        	...
            Movement->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f); 
        }
        ```

        - 무브먼트 컴포넌트는 액터의 현재 위치와 관계없이 액터에 지정된 움직임 기능을 제공한다.
        - 무브먼트 컴포넌트는 기능만 제공하는 액터 컴포넌트이고 필수적인 컴포넌트를 씬 컴포넌트라고 한다. (씬 컴포넌트는 액터 컴포넌트의 상속을 받는다.)

  - 프로젝트 재구성

    - 언리얼 프로젝트에서 액터 제거 방법
      - Source 폴더에서 관련 파일을 지우고 다시 비주얼 스튜디오 프로젝트를 재생성한다.
        1. 게임 프로젝트의 Source 폴더 내에 위치한 ArenaBattle 폴더에 있는 Fountain.h와 cpp 파일을 삭제한다. 이때 반드시 탐색기에서 관련파일을 제거해야 한다.
        2. 게임 프로젝트의 uproject 파일을 우클릭하고 Generate Visual Studio project files 메뉴를 선택해 비주얼 스튜디오 프로젝트를 재생성한다.

- 게임플레이 프레임워크

  > 언리얼 엔진에서 게임이 시작되려면 게임의 규칙, 게임에 입장하는 플레이어, 플레이어가 조종하는 액터가 있어야 함. 이를 대표하는 액터를 각각 게임 모드, 플레이어 컨트롤러, 폰 이라고 한다.

  - 게임 모드

    - 언리얼 엔진에서 게임 만드는 작업은 크게 두 가지로 구분

      1. 레벨을 구성하는 작업
      2. 게임 플레이를 설계하는 작업

    - 게임플레이 프레임워크 >> 다양한 게임의 장르와 멀티 플레이까지 수용할 수 있도록 복잡하고 방대한 구조를 가지고 있음.

    - 미리 폰을 만들어 두는 것이 아니라 클래스 정보만 저장해놓고 플레이어가 입장할 때마다 해당 클래스 정보를 기반으로 폰을 생성하는 것이 합리적임

      - 언리얼 오브젝트의 클래스 정보는 언리얼 헤더 툴에 의해 자동으로 생성되며, 언리얼 오브젝트마다 자동으로 생성된 StaticClass라는 스태틱 함수를 호출해 가져올 수 있다.

      ```c++
      //ABGameMode.cpp

      #include "ABGameMode.h"
      #include "ABPawn.h"

      AABGameMode::AABGameMode()
      {
        DefaultPawnClass = AABPawn::StaticClass();
      }
      ```

  - 플레이어의 입장

    - 게임모드는 플레이어가 입장할 때 플레이어 컨트롤러라는 액터를 함께 배정한다.

    - 플레이어 컨트롤러 : 게임 속 플레이어와 1:1로 소통하며 폰을 조종하는 역할을 맡는다. 변경 불가

    - 폰 : 플레이어 컨트롤러에게 조종당하는 액터. 플레이어는 플레이어 컨트롤러를 통해 현재 조종하는 폰을 버리고 다른 폰을 조종할 수 있다.

    - 빙의 : 플레이어 컨트롤러를 통해 폰을 조종하는 행위

    - 플레이 버튼 누를 시

      1. 플레이어 컨트롤러 생성
      2. 플레이어 폰의 생성
      3. 플레이어 컨트롤러가 플레이어 폰을 빙의
      4. 게임 시작

    - 로그인 : 플레이어가 게임에 입장하는 것

    - 플레이어가 로그인 완료 시 게임 모드의 PostLogin 이벤트 함수가 호출되는데, 이 함수 내부에서는 플레이어가 조종할 폰을 생성하고 플레이어 컨트롤러가 해당 폰에 빙의하는 작업이 이뤄진다.

    - 폰과 플레이어 컨트롤러가 생성되는 시점은 각 액터의 PostInitializeComponents 함수로 파악할 수 있고, 빙의를 진행하는 시점은 플레이어 컨트롤러의 Possess, 폰의 PossessedBy 함수로 파악할 수 있음

      ```c++
      //ABGameMode.h
      virtual void PostLogin(APlayerController* NewPlayer) override;

      //ABGameMode.cpp
      void AABGameMode::PostLogin(APlayerController * NewPlayer)
      {
        ABLOG(Warning, TEXT("PostLogin Begin"));
        Super::PostLogin(NewPlayer);
        ABLOG(Warning, TEXT("PostLogin End"));
      }

      //ABPlayerController.h
      virtual void PostInitializeComponent() override;
      virtual void Possess(APawn* aPawn) override;

      //ABPlayerController.cpp
      void AABPlayerController::PostInitializeComponents()
      {
        Super::PostInitializeComponents();
        ABLOG_S(Warning);
      }

      void AABPlayerController::Possess(APawn * aPawn)
      {
        ABLOG_S(Warning);
        Super::Possess(aPawn);
      }

      //ABPawn.h
      virtual void PostInitializeComponents() override;
      virtual void PossessedBy(AController* NewController) override;

      //ABPawn.cpp
      void AABPawn::PostInitalizeComponents()
      {
        Super::PostInitializeComponents();
        ABLOG_S(Warning);
      }

      void AABPawn::PossessedBy(AController * NewController)
      {
        ABLOG_S(Warning);
        Super::PossessedBy(NewController);
      }
      ```

- 폰의 제작과 조작

  > 생성된 폰을 직접 조작하기 위해 프로젝트의 입력을 설정하고, 입력의 신호 값을 폰의 움직임으로 변환하도록 언리얼 엔진이 제공하는 폰 무브먼트 컴포넌트를 사용한다. 애니메이션은 블루프린트를 이용한다.

  - 폰의 구성 요소

    - 인간형 폰

      - 시각적 요소 : 애니메이션 재생하도록 리깅 데이터를 추가한 메시 = 스켈레탈 메시, 이를 관리하는 컴포넌트는 스켈레탈 메시 컴포넌트임
      - 충돌 요소 : 캡슐 컴포넌트 사용
      - 움직임 요소 :  FloatingPawnMovement와 CharacterMovement라는 두 가지 폰 무브먼트 컴포넌트를 제공한다.
      - 내비게이션 : 폰은 언리얼 엔진의 내비게이션 시스템과 연동돼 있어서 목적지를 알려주면 스스로 목적지까지 이동하는 길 찾기 기능을 가지고 있다.
      - 카메라 출력 : 빙의할 때 자동으로 폰에 부착된 카메라의 상을 플레이어 화면으로 전송함

    - 사용할 컴포넌트

      - Capsule : 폰의 움직임을 담당하는 충돌 컴포넌트.
      - SkeletalMesh : 캐릭터 애셋을 보여주고 추가로 애니메이션도 담당함.
      - FloatingPawnMovement : 플레이어의 입력에 따라 캐릭터가 움직이도록 설정해주는 컴포넌트. 중력 고려하지 않음
      - SpringArm : 삼인칭 시점으로 카메라 구도를 편리하게 설정할 수 있는 부가 컴포넌트임.
      - Camera : 게임 세계의 화면을 플레이어 화면으로 전송함

      ```c++
      //ABPawn.h
      #include "GameFramework/FloatingPawnMovement.h"

      UPROPERTY(VisibleAnywhere, Category = Collision)
      		UCapsuleComponent* Capsule;

      UPROPERTY(VisibleAnywhere, Category = Visual)
      	USkeletalMeshComponent* Mesh;
      	
      UPROPERTY(VisibleAnywhere, Category = Movement)
      	UFloatingPawnMovement* Movement;

      UPROPERTY(VisibleAnywhere, Category = Camera)
      	USpringArmComponent* SpringArm;

      UPROPERTY(VisibleAnywhere, Category = Camera)
      	UCameraComponent* Camera;
      //ABPawn.cpp
      Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
      	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
      	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
      	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
      	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

      	RootComponent = Capsule;
      	Mesh->SetupAttachment(Capsule);
      	SpringArm->SetupAttachment(Capsule);
      	Camera->SetupAttachment(SpringArm);

      	Capsule->SetCapsuleHalfHeight(88.0f);
      	Capsule->SetCapsuleRadius(34.0f);
      	Mesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
      	SpringArm->TargetArmLength = 400.0f;
      	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

      	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
      		
      	if (SK_CARDBOARD.Succeeded())
      	{
      		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
      	}
      ```

  - 폰의 조작

    - 폰에 입력에 대한 처리 로직과 폰 무브먼트 설정함.

    - 먼저 프로젝트 세팅에서 Axis Mappings와 Action Mappings를 설정한다.

    - 폰의 게임 로직에서 이를 사용하도록 코드로 작성한다.

      - 입력 설정을 위해 InputComponent라는 언리얼 오브젝트를 제공한다.
        - InputComponent는 BindAxis와 BindAction 두 함수를 제공한다.
      - 폰의 SetupInputComponent 함수에서 입력 설정과 멤버 함수를 연결한다. 

      ```c++
      //ABPawn.h	
      void UpDown(float NewAxisValue);
      void LeftRight(float NewAxisValue);

      //ABPawn.cpp
      void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
      {
      	Super::SetupPlayerInputComponent(PlayerInputComponent);

      	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABPawn::UpDown);
      	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABPawn::LeftRight);
      }

      void AABPawn::UpDown(float NewAxisValue)
      {
      	ABLOG(Warning, TEXT("%f"), NewAxisValue);
      }

      void AABPawn::LeftRight(float NewAxisValue)
      {
      	ABLOG(Warning, TEXT("%f"), NewAxisValue);
      }
      ```

      - 폰 무브먼트 컴포넌트의 AddMovementInput 함수로 움직임 구현

      - 추가적으로 이동할 방향을 WorldDirection에 지정해야 함.

        - 월드 좌표계 기준으로 액터의 전진 방향의 벡터 데이터는 GetActorForwardVector 함수를 사용해 가져올 수 있다. 이 값을 WorldDirection 인자에 넘겨준다.

          ```c++
          void AABPawn::UpDown(float NewAxisValue)
          {
          	AddMovementInput(GetActorForwardVector(), NewAxisValue);
          }

          void AABPawn::LeftRight(float NewAxisValue)
          {
          	AddMovementInput(GetActorRightVector(), NewAxisValue);
          }
          ```

        - 플레이 버튼을 눌러 콘텐츠를 테스트할 때 뷰포트를 클릭해 포커스를 잡아야 하는 문제 해결

          ```c++
          //ABPlayerController.h
          virtual void BeginPlay() override;

          //ABPlayerController.cpp
          void AABPlayerController::BeginPlay()
          {
          	Super::BeginPlay();

          	FInputModeGameOnly InputMode;
          	SetInputMode(InputMode);
          }
          ```

    - 애니메이션 설정

      - BeginPlay 함수에서 애니메이션 애셋을 로딩하는 기능을 추가한다. 게임 실행 중 에셋을 로드하는 명령어 : LoadObject<타입>

        ```c++
        void AABPawn::BeginPlay()
        {
        	Super::BeginPlay();
        	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
        	UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Book/Animations/WarriorRun.WarriorRun"));
          	
          	if (AnimAsset != nullptr)
        	{
        		Mesh->PlayAnimation(AnimAsset, true);
        	}
        }
        ```

      - 블루프린트는 애님 그래프 로직에 따라 동작하는 캐릭터 애니메이션 시스템을 구동시킴. 이러한 애니메이션 시스템은 C++ 프로그래밍의 애님 인스턴스 라는 클래스로 관리된다.

      - 스켈레탈 메시가 이 애니메이션 블루프린트를 실행시키려면 블루프린트 애셋의 클래스 정보를 애님 인스턴스 속성에 지정해줘야 한다.

      - 스켈레탈 메시 컴포넌트에 애니메이션 블루프린트의 클래스 정보를 등록하면, 컴포넌트는 인스턴스를 생성해 애니메이션을 관리하도록 동작한다. 그래서 애셋의 경로에 _C를 추가로 붙여 클래스 정보를 가져오는 경로를 생성하고, 이를 스켈레탈 메시 컴포넌트의 애니메이션 블루프린트 클래스에 등록한다.

        ```C++
        //ABPawn.cpp
        AABPawn::AABPawn()
        {
         	...
              
        	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

        	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBluprint.WarriorAnimBluprint_C"));

        	if (WARRIOR_ANIM.Succeeded())
        	{
        		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
        	}
        }
        ```

        ​

      ​