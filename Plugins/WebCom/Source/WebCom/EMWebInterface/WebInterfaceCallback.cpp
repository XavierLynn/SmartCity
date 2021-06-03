// Copyright 2021 x.Lin. All Rights Reserved
#include "WebInterfaceCallback.h"
#include "ComWebBrowser.h"


FWebInterfaceCallback::FWebInterfaceCallback()
{
	//
}

FWebInterfaceCallback::FWebInterfaceCallback( TWeakObjectPtr<UComWebBrowser> Interface, const FString& Callback )
{
	MyInterface = Interface;
	MyCallback  = Callback;
}

bool FWebInterfaceCallback::IsValid() const
{
	if ( MyInterface.IsValid() )
		return !MyCallback.IsEmpty();

	return false;
}

void FWebInterfaceCallback::Call( const FJsonLibraryValue& Data )
{
	if ( !MyInterface.IsValid() || MyCallback.IsEmpty() )
		return;

	if ( Data.GetType() != EJsonLibraryType::Invalid )
		MyInterface->ExecuteJavascript( FString::Printf( TEXT( "ue.interface[%s](%s)" ),
			*FJsonLibraryValue( MyCallback ).Stringify(),
			*Data.Stringify() ) );
	else
		MyInterface->ExecuteJavascript( FString::Printf( TEXT( "ue.interface[%s]()" ),
			*FJsonLibraryValue( MyCallback ).Stringify() ) );
}
