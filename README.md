# Movement Demo
This is an advanced movement demo featuring vaulting, dodging, crouching, and climbing; powered by a combination of Motion Warping and GAS. Made in UE 5.3.

## Abilities
### Vaulting
The vaulting ability uses a series of line traces to plot a path over a potentially vaultable surface. If a valid path is found, points along the path are isolated and added to the motion warping component on the player.
The player then moves and animates along that path to create the vaulting system. Vaulting can be activated while standing or crouching. This ability supports multiplayer gameplay.
### Dodging
Dodging is a simple ability where an animation montage is played and reset upon the montage's completion to allow the player to dodge again. This ability supports multiplayer gameplay.
### Climbing
The climbing system utilitzes a series of line traces to determine the best course of action while climbing. While climbing on a surface, three line traces are being utilitzed for edge and ledge detection.
If an edge is found on the left or right, the player will no longer be able to move in that direction. If a ledge is detected above the player, they will grab the edge and pull themselves up utilizing the player's motion warping component.
Ledge grapping can also be used on taller structures to pull the character up without directly climbing or jumping. This ability currently does not support multiplayer gameplay.
## Ability Tasks
### AbilityTaskOnTick
A simple ticking task to allow for tick functionality on gameplay abilities. Useful for C++ timelines and continuous tracking.
### PlayMontageAndWaitForEvent
A combination of two built in ability tasks, this task will play a montage and then fire off events at appropriate times. Useful for when you need access to a more robust play animation montage function.
