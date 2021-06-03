// Copyright 2021 x.Lin, Inc. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "SWebBrowser.h"
#include "JsonLibrary/Public/JsonLibrary.h"
#include "WebCom/EMWebInterface/WebInterfaceCallback.h"
#include "ComWebBrowser.generated.h"


UCLASS()
class WEBCOM_API UComWebBrowser : public UWidget
{
	GENERATED_UCLASS_BODY()
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInterfaceEvent, const FName, Name, FJsonLibraryValue, Data, FWebInterfaceCallback, Callback);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);

	/**
	 * Load the specified URL
	 *
	 * @param NewURL New URL to load
	 */
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser")
		void LoadURL(FString NewURL);

	/**
	 * Load a string as data to create a web page
	 *
	 * @param Contents String to load
	 * @param DummyURL Dummy URL for the page
	 */
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser")
		void LoadString(FString Contents, FString DummyURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser")
		void ExecuteJavascript(const FString& ScriptText);

	/**
	 * Get the current title of the web page
	 */
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser")
		FText GetTitleText() const;

	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser")
		FString GetUrl() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "WEBCOM Browser|Event")
		FOnUrlChanged OnUrlChanged;

	/** Called when a popup is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "WEBCOM Browser|Event")
		FOnBeforePopup OnBeforePopup;


	// Call ue.interface.function(data) in the browser context.
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser|Communication", meta = (AdvancedDisplay = "Data", AutoCreateRefTerm = "Data"))
		void Call(const FString& Function, const FJsonLibraryValue& Data);
	// Bind an object to ue.name in the browser context.
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser|Communication")
		void Bind(const FString& Name, UObject* Object);
	// Unbind an object from ue.name in the browser context.
	UFUNCTION(BlueprintCallable, Category = "WEBCOM Browser|Communication")
		void Unbind(const FString& Name, UObject* Object);

	// Called with ue.interface.broadcast(name, data) in the browser context.
	UPROPERTY(BlueprintAssignable, Category = "WEBCOM Browser|Communication")
		FOnInterfaceEvent OnInterfaceEvent;

public:

	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	class UWebInterfaceObject* MyObject;


protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category = Appearance)
		FString InitialURL = "google.com";

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category = Appearance)
		bool bSupportsTransparency;

protected:
	TSharedPtr<class SWebBrowser> ComWebBrowser;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	void HandleOnUrlChanged(const FText& Text);
	bool HandleOnBeforePopup(FString URL, FString Frame);

};