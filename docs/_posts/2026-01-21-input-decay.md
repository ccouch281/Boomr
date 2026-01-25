---
layout: post
title: Input Decay
date: 2026-01-21
category: Deep Dive
custom_css: |
    .demo-container {
        background: #000;
        border: 1px solid var(--border-color);
        border-radius: 8px;
        padding: 1rem;
        margin: 2rem 0;
        text-align: center;
    }

    canvas {
        background: #111;
        max-width: 100%;
        border: 1px solid #333;
    }

    .controls {
        margin-top: 1rem;
        display: flex;
        gap: 1rem;
        justify-content: center;
    }

    button {
        background: var(--accent-primary);
        color: white;
        border: none;
        padding: 0.5rem 1rem;
        border-radius: 4px;
        cursor: pointer;
        font-weight: 600;
    }

    button:hover {
        background: var(--accent-hover);
    }
---

There is a subtle but destructive bug plaguing many First Person Shooter engines (even modern ones), and I've decided to call it **Input Erosion**.

It helps explain why some games feel "crisp" and "raw" while others feel "floaty," "inconsistent," or like there is a layer of vaseline over your aim.

## The Journey of an Input
To understand the problem, we have to look at what actually happens when you move your mouse or controller stick. Your device doesn't speak in "degrees" or "pixels." It speaks in **Counts** (sometimes called Mickeys for mice, or raw axis values for controllers).

This is an **Integer**. A whole number. You moved `+1` unit. Not `0.999`. Not `1.00001`. Exactly `1`.

Ideally, this integer should be treated with reverence. It should be added to a "master buffer" that tracks your total accumulated movement. But that's not what many engines do.

## Deep Dive: From Physical to Digital

Before we even get to the game engine, your input goes through a fascinating hardware journey. Let's look at how "reality" becomes "ones and zeros."

### 1. The Joystick (Analog to Digital)
A joystick stick is essentially two **Potentiometers** (variable resistors) mounted at 90-degree angles.
*   **The Physics:** As you tilt the stick, you are physically moving a wiper across a resistive track. This changes the resistance in the circuit.
*   **The Voltage:** This changing resistance creates a **Voltage Divider**. If your controller runs on 3.3V, pushing the stick all the way forward might output 3.3V, while pulling it back outputs 0V. Center is roughly 1.65V.
*   **The Conversion (ADC):** This analog voltage signal is fed into an **Analog-to-Digital Converter (ADC)** chip on the controller's board. The ADC samples the voltage and assigns it a binary value.
    *   A typical 16-bit ADC might map 0V to `0` and 3.3V to `65535`.
    *   The "center" resting point isn't perfectly 0. It's a specific integer, like `32768`.

**The Key Takeaway:** Even "Analog" sticks are actually sending **Integers** to your PC. A movement of "0.5" doesn't exist. It is a discrete step from `32768` to `32769`.

### 2. The Mouse (Optical Flow)
Modern optical mice are tiny high-speed cameras taking thousands of pictures of your mousepad every second.
*   **The Optical Engine:** An Image Signal Processor (ISP) compares "Frame A" to "Frame B".
*   **Cross Correlation:** It calculates the offset between features in the surface texture.
*   **The Output:** It spits out `Delta X` and `Delta Y`. These are count values. Again, **Integers**.

In both cases, we start with precise, quantized data. The hardware has done its job perfectly. The data loss happens when we (the developers) decide to ignore this binary nature.

## The "Destructive Read" Trap
Here is the common, incorrect pattern that causes Input Erosion:

1.  **Read:** The engine asks the OS for the input delta since the last frame. Let's say it's `3` counts.
2.  **Convert:** The engine converts that `3` (int) into `3.0f` (float).
3.  **Scale:** It multiplies by sensitivity (e.g., `0.05`). Result: `0.15f`.
4.  **Apply:** It adds `0.15f` to the player's view rotation.
5.  **Zero:** <span style="color: #ff6b6b; font-weight: bold;">CRITICAL ERROR:</span> It resets the delta to `0` for the next frame.

Why is this bad? Because **Floats are lossy**.

When you take a precise integer, smash it into a float, do math on it, apply it to a rotation (which is likely a large number), and then *discard the original integer source*, you are permanently deleting tiny fractions of data.

If you repeat this process thousands of times a second (high polling rate mice) over thousands of frames, these tiny errors accumulate. Your crosshair literally ends up in a different place than your hand movement dictated.

## The Solution: Non-Destructive Buffering
The fix is simple, but requires a mindset shift. **Don't consume the delta. Accumulate it.**

Instead of asking "How much did I move *this frame*?", you should track "How much have I moved *total, ever*?".

By keeping the source of truth as an Integer, you ensure that `10,000` counts to the right is *always* exactly the same rotation, no matter how many frames it took to get there.

## Lessons from the Masters: Quake & Doom
It's no coincidence that the games celebrated for the best "game feel" in history handled this correctly. If we look at the source code for the original *Quake* and *Doom*, we see they treated input as sacred integers.

### Quake (id Tech 2)
In `in_win.c`, Quake didn't immediately turn mouse input into a float. It used static **accumulators**.

```c
static int mx_accum, my_accum;

void IN_MouseMove (usercmd_t *cmd)
{
    int mx_delta = GetRawMouseX();
    mx_accum += mx_delta;
}
```

### Doom (id Tech 1)
Similarly, *Doom* used an event system (`event_t`) that stored input data strictly as integers.

```c
typedef struct
{
    eventtype_t type;
    int data1;
    int data2;
    int data3;
} event_t;
```

These engines respected the hardware. They recognized that the mouse is a digital device sending precise counts, not a fuzzy analog stick sending a voltage signal (though even that, as we discussed, becomes an integer eventually).

<!-- Interactive Demo -->
<div class="demo-container">
    <h3>Drift Simulation</h3>
    <p>Visualizing 1000 frames of accumulated error.</p>
    <canvas id="driftCanvas" width="800" height="300"></canvas>
    <div class="controls">
        <button onclick="resetSimulation()">Re-run Simulation</button>
    </div>
</div>

<script>
    const canvas = document.getElementById('driftCanvas');
    const ctx = canvas.getContext('2d');
    let frame = 0;
    let idealPos = 0;
    let erodedPos = 0;
    const sensitivity = 0.123456789; // Using a complex sensitivity to exacerbate float errors

    // Simulation state
    const history = {
        ideal: [],
        eroded: []
    };

    function runSimulation() {
        // Simulate 1 tick
        const mouseInput = 5; // Steady movement of 5 counts per tick

        // Ideal: Integer Accumulation
        // We verify by calculating position from total counts
        idealPos += mouseInput;

        // Eroded: Float Conversion and Accumulation
        // We simulate the loss by casting to float 32-bit (simulated here)
        // In JS numbers are double precision so we have to artificially introduce error
        // or just show the drift from sensitivity multiplication truncation.

        // Actually, a better illustration of the "Zero-Reset" problem:
        // The error comes when the accumulated rotation (large float) 
        // has a small delta (small float) added to it.
        // Floating point addition eats precision when adding small numbers to large numbers.

        // Let's simulate a large current rotation
        let currentRotationEroded = 10000.0 + (erodedPos * sensitivity);
        let deltaRotation = Math.fround(mouseInput * sensitivity); // fround gives us single precision float simulation

        // The destructive act:
        let nextRotation = Math.fround(currentRotationEroded + deltaRotation);

        // Extract what "pos" we think we are at
        erodedPos = (nextRotation - 10000.0) / sensitivity;

        history.ideal.push(idealPos);
        history.eroded.push(erodedPos);

        draw();
        frame++;
        if (frame < 1000) {
            requestAnimationFrame(runSimulation);
        }
    }

    function draw() {
        ctx.fillStyle = '#161b22';
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        if (history.ideal.length < 2) return;

        // Normalize to fit screen
        const maxVal = history.ideal[history.ideal.length - 1];
        const scaleY = (canvas.height - 40) / maxVal;
        const scaleX = canvas.width / 1000;

        // Draw Ideal (Blue)
        ctx.strokeStyle = '#58a6ff';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(0, canvas.height - 20);
        for (let i = 0; i < history.ideal.length; i++) {
            ctx.lineTo(i * scaleX, canvas.height - 20 - (history.ideal[i] * scaleY));
        }
        ctx.stroke();

        // Draw Eroded (Red)
        ctx.strokeStyle = '#ff6b6b';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(0, canvas.height - 20);
        for (let i = 0; i < history.eroded.length; i++) {
            ctx.lineTo(i * scaleX, canvas.height - 20 - (history.eroded[i] * scaleY));
        }
        ctx.stroke();

        // Legend
        ctx.fillStyle = '#58a6ff';
        ctx.fillText("Ideal (Pure Integer)", 20, 30);
        ctx.fillStyle = '#ff6b6b';
        ctx.fillText("Eroded (Float Accumulation)", 20, 50);
    }

    function resetSimulation() {
        frame = 0;
        idealPos = 0;
        erodedPos = 0;
        history.ideal = [];
        history.eroded = [];
        runSimulation();
    }

    // Start
    resetSimulation();
</script>

## The Math Battle: Matrix vs. Quaternion
As we start accumulating these raw values, we face another challenge: How do we apply them?

We've all heard of "Gimbal Lock"—the nemesis of Euler angles. But even standard Matrices have their issues when doing thousands of incremental updates. This brings us to a showdown between two mathematical heavyweights.

![Matrix vs Quaternion Fight](../assets/images/matrix_vs_quaternion.png)

*We'll dive deeper into this mathematical showdown in the next section...*

