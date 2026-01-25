#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"

struct alignas(16) FIntPoint64
{
	int64 X;
	int64 Y;

	FORCEINLINE FIntPoint64() : X(0), Y(0) {}
	FORCEINLINE FIntPoint64(int64 InX, int64 InY) : X(InX), Y(InY) {}

	// Returns true if the buffer is large enough that float precision might start to fail (~10^7)
	FORCEINLINE bool ReachesPrecisionLimit() const { return FMath::Abs(X) > 10000000 || FMath::Abs(Y) > 10000000; }
};

class BOOMRRAWINPUT_API FRawInputMessageHandler : public IWindowsMessageHandler
{
public:
	virtual bool ProcessMessage(HWND WindowHandle, uint32 Message, WPARAM WordParameter, LPARAM LongParameter, int32& OutResult) override;

	/** Returns the absolute running total of counts since start or last reset.
	 *  Treat this as the high-precision "Source of Truth" for your orientation. 
	 */
	FIntPoint64 GetTotalAccumulation() const;

	/** Returns the absolute total scroll counts. */
	FIntPoint64 GetScrollAccumulation() const;

	/** Forcefully sets the accumulation to a specific value.
	 *  Use this to synchronize the mouse buffer when the Engine sets the camera rotation (e.g. Spawning).
	 */
	void SetAccumulation(int64 NewX, int64 NewY);

	/** Resets scroll accumulation to zero. */
	void ResetScroll();

	/** Gets the orientation counts at the exact moment the left mouse button was last pressed. 
	 *  Returns true if there is a new pending click to process.
	 */
	bool ConsumeClick(FIntPoint64& OutCounts);

private:
	// Permanent accumulation buffer. int64 will not overflow for ~300,000 years.
	alignas(16) volatile int64 MouseBufferX = 0;
	alignas(16) volatile int64 MouseBufferY = 0;

	// Scroll wheel accumulation (Vertical and Horizontal)
	alignas(16) volatile int64 ScrollBufferX = 0;
	alignas(16) volatile int64 ScrollBufferY = 0;

	// Sub-tick click tracking
	alignas(16) volatile int64 ClickBufferX = 0;
	alignas(16) volatile int64 ClickBufferY = 0;
	alignas(16) volatile int32 bHasPendingClick = 0;
};
#endif
