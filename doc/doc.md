# Documentation

Documentation covers following in detail:

- A sample program structure
- Different components involved
- Defining concepts used

## A sample program structure

README covers a sample program. A very simple program may have following structure:

1. Define execution context
2. Define the camera configuration
3. Define the world
4. Define the image
5. Run the algorithm

We would cover these things in detail in upcoming sections.

## Different components involved

### Execution Context

Speaking on a high level, ray tracer needs to run some algorithm to generate
the image. Now we can run this algorithm on different execution context like
- Single Thread
- Thread Pool
- GPU
- On some remote machine
based on our usecase.

Execution context determines where the rendering algorithm runs.
In code we call the reference to execution context as **Scheduler**.

### Camera && Camera Orientation

We look the world through eyes. As our eyes move or change its orientation,
how the world looks changes. In terms of ray tracing camera acts as our eyes and
camera orientation as orientation of our eyes (e.g., we are facing forward by
our eye is looking upwards).

There is subtle difference between how both works. In case of eye, we capture
the all the rays coming to us. However, this is very computationally
expensive. In case of camera, we send the rays in opposite direction from
camera and observe it after iteraction with world.

### World

World contains different objects. Rays from camera hit these objects and
deflects and change color. World itself can be treated as object. In terms
of code we call objects as **SceneObject**. When ray hits the scene object, it
hits some part of hit. That part that got hit is called **HitObject**.

### Image

Image can be treated as matrix of colors (pixels). Its the 2d representation of
how we see the 3d world. This is the output of rendering algorithm.

### Rendering algorithm

Rendering algorithm takes execution context, camera, camera orientation, world
and image (for filling output colors) as argument and runs the algorithm on
execution context and fills the image with how camera sees the world.

## Defining concepts used
