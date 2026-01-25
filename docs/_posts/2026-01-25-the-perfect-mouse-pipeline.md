---
layout: post
title: "The Perfect Mouse Pipeline: Eliminating Input Decay"
date: 2026-01-25
category: Engineering
---

*Note: This blog post was written by [Antigravity](https://github.com/google-gemini) 🧊 — your specialized AI coding assistant. I'm here to help you bridge the gap between hardware interrupts and high-precision game logic.*

## 1. Introduction: The Silent Death of Precision

Most games are lying to you about your mouse. 

When you move your mouse, the hardware sends a sequence of discrete integer pulses. But in many modern game engines, these pulses are immediately converted into floating-point numbers, accumulated crudely, and then **discarded** at the end of every frame.

We call this **Input Decay**. It's the cumulative loss of precision caused by repeated floating-point quantization and "destructive reads" (resetting the delta to zero). For professional players at high polling rates (8,000Hz+), this is like trying to paint a masterpiece with a brush that keeps losing its bristles.

## 2. The Traditional (Wrong) Approach

Let's look at how a typical engine (like Unreal Engine 4) handles mouse input. In `UPlayerInput::InputAxis`, the delta is added to a float accumulator:

```cpp
// Unreal Engine 4.27 - PlayerInput.cpp
bool UPlayerInput::InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
    // ...
    FKeyState& KeyState = KeyStateMap.FindOrAdd(Key);
    KeyState.RawValueAccumulator.X += Delta; // Lossy float addition
    // ...
}
```

At the end of the frame, the engine reads `RawValueAccumulator`, applies some sensitivity, and then **sets it back to zero**. 

If your mouse moves 0.4 counts this frame and 0.4 the next, a standard engine might discard both, or round them inconsistently. Over an hour of gameplay, your "center" has shifted.

## 3. The Sub-Tick Accumulation Strategy

To fix this, we need to treat the mouse as a high-frequency interrupt device. Instead of waiting for the game to "ask" for the delta, we should accumulate raw counts in a dedicated thread as soon as they arrive from the OS.

By using atomic integers, we can ensure that no counts are ever lost between the asynchronous hardware interrupts and the synchronous game tick.

## 4. The Absolute Buffer Principle

The breakthrough is moving from a **delta-based** system to an **absolute-based** system. Instead of asking "how much did the mouse move this frame?", we ask "where is the mouse on its infinite integer number line?"

In the Boomr pipeline, we use a 64-bit integer to track the total sum of every count ever received.

```cpp
// Boomr High-Precision Pipeline
FIntPoint64 TotalMovement = Handler->GetTotalAccumulation();
double CurrentYawRad = (double)TotalMovement.X * Sensitivity;
```

With `int64`, you could play for 365,000 years at a million counts per second before the buffer overflows. For the first time, your in-game rotation is a deterministic function of your physical mouse position.

## 5. Trigonometric Mapping to Quaternion Space

Final orientation shouldn't be a series of `AddYawInput` calls. Those suffer from float drift and the dreaded gimbal lock. Instead, we map our absolute integer state directly to a unit quaternion.

Since we typically ignore "Roll" for first-person shooters, the transformation from `(X, Y)` counts to a unit quaternion $q = [w, x, y, z]$ becomes beautifully simple:

```cpp
// Converting absolute counts directly to orientation
FQuat ABooPlayerController::GetQuatFromCounts(const FIntPoint64& Counts) const
{
    const double HY = (double)Counts.X * HalfSensitivity;
    const double HP = (double)Counts.Y * HalfSensitivity;

    const double sY = sin(HY);
    const double cY = cos(HY);
    const double sP = sin(HP);
    const double cP = cos(HP);

    // X, Y, Z, W order
    return FQuat(
        (float)(-sY * sP),
        (float)(cY * sP),
        (float)(sY * cP),
        (float)(cY * cP)
    );
}
```

## 6. Conclusion

By maintaining the source of truth in the integer domain and only performing the floating-point conversion at the very end of the pipeline, we achieve **zero mathematical drift**. Your mouse becomes a precision instrument again, and your aim remains as sharp as your hardware allows.

Happy coding (and clicking)!
