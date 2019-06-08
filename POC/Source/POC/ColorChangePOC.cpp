// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorChangePOC.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"


// Sets default values for this component's properties
UColorChangePOC::UColorChangePOC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UColorChangePOC::BeginPlay()
{
	Super::BeginPlay();
	FString ObjectName = Owner->GetName();
	FString ObjectPos = Owner->GetTransform().GetLocation().ToString();
	UE_LOG(LogTemp, Warning, TEXT("%s is at %s"), *ObjectName, *ObjectPos);
	// ...
	connected = ConnectSocket();
	if (connected)
	{
		FString this_port = FString::FromInt(Socket->GetPortNo());
		UE_LOG(LogTemp, Warning, TEXT("Successfuly connected socket to loopback using port %s!"), *this_port);
		SendToSocket();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not connect socket."));
	}
}

bool UColorChangePOC::ConnectSocket()
{
	// Lets create our socket. Some adjustments were needed from this example.
	// https://wiki.unrealengine.com/Third_Party_Socket_Server_Connection
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetLoopbackAddress();
	addr->SetPort(12111);

	// Connect returns true if connection was successful, false otherwise
	return Socket->Connect(*addr);
}

void UColorChangePOC::SendToSocket()
{
	SocketIOSuccess = SendStringOverSocket(TEXT("Hello from Unreal!"));
	if (SocketIOSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully sent a message to the Eiffel server!"));
	}
	message_size = 0;
	SocketIOSuccess = false;
}

bool UColorChangePOC::SendStringOverSocket(FString this_message)
{
	// NOTE! The Eiffel server expects a newline (\n for uint8) at the end of messages.
	this_message += TEXT("\n");
	TCHAR *serializedChar = this_message.GetCharArray().GetData();
	message_size = FCString::Strlen(serializedChar);
	int32 sent = 0;

	return Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), message_size, sent);
}

bool UColorChangePOC::ReceiveFromSocket()
{
	FString ResponseSize = FString::FromInt(message_size);
	UE_LOG(LogTemp, Warning, TEXT("Response received from server of size %s"), *ResponseSize);
	// Slightly modified the code from 
	// https://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_%28Full_Code_Sample%29
	// Create a binary array to hold the response
	TArray<uint8> ReceivedData;
	uint32 Size;
	while (Socket->HasPendingData(Size))
	{
		ReceivedData.Init(uint8(0), FMath::Min(message_size, 65507u));

		int32 Read = 0;
		// Recv: https://docs.unrealengine.com/en-US/API/Runtime/Sockets/FSocket/Recv/index.html
		Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}
	if (ReceivedData.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Data was not successfully read from the socket buffer."));
		return false;
	}
	FString size_received = FString::FromInt(ReceivedData.Num());
	UE_LOG(LogTemp, Warning, TEXT("Total data read: %s"), *size_received);
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	UE_LOG(LogTemp, Warning, TEXT("Response from Server: %s"), *ReceivedUE4String);
	return true;
}

//Rama's String From Binary Array
FString UColorChangePOC::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	BinaryArray.Add(0); // Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

void UColorChangePOC::CloseServerAndSocket()
{
	// Server will close when client message is "quit"
	SocketIOSuccess = SendStringOverSocket(TEXT("quit"));
	connected = false;
	// Socket->Close();
}

void UColorChangePOC::ChangeColor()
{
	// Eiffel requested a color change. Flip the sign and log in Unreal
	ColorState *= -1;
	FString StateString = FString::FromInt(ColorState);
	UE_LOG(LogTemp, Error, TEXT("Color Change requested. State is now %s"), *StateString);

	// some record keeping to track when to automatically change state after a time delay
	LastColorChangeTime = GetWorld()->GetTimeSeconds();
}


// Called every frame
void UColorChangePOC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Are we still connected to the server?
	if (connected)
	{
		// Has the server sent us a response message?
		if (Socket->HasPendingData(r_message_size)) {
			// Try reading the socket buffer
			SocketIOSuccess = ReceiveFromSocket();
			// Was the buffer successfully read?
			if (SocketIOSuccess) {
				// Close down the Server and the socket.
				CloseServerAndSocket();
			}
		}
	}

	// Some testing code to automatically swap colors after a set period of time has elapsed.
	if (GetWorld()->GetTimeSeconds() - LastColorChangeTime >= ColorChangeDelay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Color change timeout reached..."));
		ChangeColor();
	}

	if (ColorState > 0)
	{
		// Broadcast serves as the trigger in Blueprint
		OnRed.Broadcast();
	}
	else
	{
		OnBlue.Broadcast();
	}
}

