# UE4

- 액터의 설계

  - C++에서 액터가 두개의 스태틱메시 컴포넌트를 가지려면 액터의 멤버 변수로 두 개의 UStaticMeshComponent 클래스의 포인터를 선언해야 함.

  - CoreMinimal.h 대신 엔진 클래스의 선언만 모아둔 EngineMinimal.h를 주로 사용한다.

  - 포인터로 선언하여 메모리를 동적으로 할당하여 대입하면, 명시적으로 객체를 소멸하여 메모리 관리를 해야 하는데 언리얼 엔진은 언리얼 실행 환경을 통해 객체가 사용되지 않으면 할당된 메모리를 자동으로 소멸시키는 기능을 제공한다.

  - 자동을 관리하기 위해서는 UPROPERTY라는 매크로를 사용해 객체를 지정해줘야 한다.

  - 모든 객체가 UPROPERTY 매크로를 사용해서 자동으로 메모리 관리를 할 수 있지는 않고, 언리얼 오브젝트라는 특별한 객체에만 사용 가능하다. - 콘텐츠를 구성하는 모든 객체

  - C++ 클래스가 언리얼 오브젝트 클래스가 되기 위해서는 규칙이 필요함

    - 클래스 선언 매크로 : 클래스 선언의 윗줄에 UCLASS라는 매크로 선언하고 클래스 내부에는 GENERATED_BODY 매크로를 선언
    - 클래스 이름 접두사 : U와 A가 있으며, 액터 클래스에는 A를 사용하고 액터가 아닌 클래스에는 U를 사용한다.
    - generated.h 헤더파일 : 소스코드를 컴파일하기 이전에 언리얼 엔진은 언리얼 헤더 툴이라는 도구를 사용해 클래스 선언을 분석하고 언리얼 실행 환경에 필요한 부가 정보를 별도의 파일에 생성한다. 이 부가 파일이 generated.h 파일이다.
    - 외부 모듈에의 공개 여부 : 윈도우의 DLL 시스템은 DLL 내 클래스 정보를 외부에 공개할지 결정하는 _declspec(dllexport)라는 키워드를 제공하는데, 언리얼 엔진에서 이 키워드를 사용하기 위해서 '모듈명\_API'라는 키워드를 클래스 선언 앞에 추가한다. 이 키워드가 없을 시 다른 모듈에서 해당 객체에 접근할 수 없다.

    ```C++
    //Fountain.h
    #pragma once

    #include "EngineMinimal.h"
    #include "GameFramework/Actor.h"
    #include "Fountain.generated.h"

    UCLASS()
    class ARENABATTLE_API AFountain : public AActor
    {
    	GENERATED_BODY()
    public:
    	virtual void Tick(float DeltaTime) override;
    	
    	UPROPERTY()
    	UStaticMeshComponent *Body;
    	
    	UPROPERTY()
    	UStaticMeshComponent *Water;
    };
    ```

    - 언리얼 엔진에서 컴포넌트를 생성하는 용도로 new가 아닌 CreateDefaultSubobject API라는 특별한 함수를 제공한다.

    ```c++
    //Fountain.cpp
    #pragma once

    #include "EngineMinimal.h"
    #include "GameFramework/Actor.h"

    #include "Fountain.h"

    AFountain::AFountain()
    {
      PrimaryActorTick.bCanEverTick = true;
      
      Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
      Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
      
      RootComponent = Body;
      Water->SetupAttachment(Body);
    }

    void AFountain::BeginPlay()
    {
      Super::BeginPlay();
    }

    void AFountain::Tick(float DeltaTime)
    {
      Super::Tick(DeltaTime);
    }
    ```

    - 루트 컴포넌트로 Body를 지정하고 Water은 Body의 자식이 되도록 설정한다.

  - 액터와 에디터 연동

    - 디테일 윈도우에서 컴포넌트의 속성을 편집하기 위해서는 컴포넌트의 선언에 특별한 키워드를 등록해줘야 한다. 멤버 변수 선언 윗줄의 UPROPERTY 매크로 안에 VisibleAnywhere라는 키워드를 추가한다.

      ```c++
      UPROPERTY(VisibleAnywhere)
      ```

    - SetRelativeLocation을 사용하면 컴포넌트의 기본 위치 값을 변경할 수 있다.

      ```c++
      Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
      ```

      - F 접두사는 언리얼 오브젝트와 관련없는 일반 c++클래스 혹은 구조체를 의미한다.

    - UPointLightComponent 클래스를 사용하여 조명기능을 사용하고, UParticleSystemComponent 클래스를 사용하여 이펙트를 사용한다.

      ```c++
      UPROPERTY(VisibleAnywhere)
      	UPointLightComponent *Light;

      UPROPERTY(VisibleAnywhere)
      	UParticleSystemComponent *Splash;
      ```

  - 객체 유형과 값 유형

    - 바이트 : uint8

    - 정수 : int32

    - 실수 : float

    - 문자열 : FString, FName

    - 구조체 : FVector, FRotator, FTransform

    - 속성의 데이터를 변경하려면 VisibleAnywhere이 아니라 EditAnywhere 키워드를 사용해야 한다.

      ```c++
      UPROPERTY(EditAnywhere, Category=ID)
        int32 ID;
      ```

    - 에셋을 불러오기 위해서 ConstructorHelpers라는 클래스의 FObjectFinder을 사용해 변수를 선언하고 경로 값을 전달한다.

      ```c++
      static ConstructorHelpers::FObjectFinder<UStaticMesh>
      		SM_BODY(TEXT("Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));

      	if (SM_BODY.Succeeded())
      	{
      		Body->SetStaticMesh(SM_BODY.Object);
      	}

      static ConstructorHelpers::FObjectFinder<UParticleSystem>
      		PS_SPLASH(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01"));
      		
      	if (PS_SPLASH.Succeeded())
      	{
      		Splash->SetTemplate(PS_SPLASH.Object);
      	}
      ```

      ​