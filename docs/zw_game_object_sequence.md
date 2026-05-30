# Game Object Sequences

This document describes the runtime sequence of each main object in Zomwar. Each object is handled by its own AK task and receives signals from the screen task, button callbacks, timers, or other object tasks.

## I. Object Summary

| Object | Task ID | Handler | Main responsibility |
|---|---|---|---|
| Archery | `AR_GAME_ARCHERY_ID` | `ar_game_archery_handle()` | Controls the player position and bow image state. |
| Arrow | `AR_GAME_ARROW_ID` | `ar_game_arrow_handle()` | Shoots arrows, moves active arrows, and restores available arrow count. |
| Meteoroid | `AR_GAME_METEOROID_ID` | `ar_game_meteoroid_handle()` | Spawns meteoroids, moves them, checks collision with arrows, and updates score. |
| Bang | `AR_GAME_BANG_ID` | `ar_game_bang_handle()` | Plays explosion animation after a meteoroid is hit. |
| Border | `AR_GAME_BORDER_ID` | `ar_game_border_handle()` | Checks level-up condition and game-over condition. |

## II. Archery Object Sequence

Archery owns the player position. The screen task initializes the Archery object when gameplay starts, then the periodic game tick posts update messages every `AR_GAME_TIME_TICK_INTERVAL`. Button callbacks post movement signals directly to the Archery task while the game is playing. Movement changes the internal `archery_y` value, and the next update copies that value into the rendered `archery.y`.

```mermaid
sequenceDiagram
    autonumber
    participant Button as Button Callback
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(Archery Game Screen)
    participant Archery as Archery Task

    Note over Screen,Archery: Game entry initializes Archery state

    Screen->>Archery: AR_GAME_ARCHERY_SETUP
    activate Archery
    Note right of Archery: Set x, y, visible,<br/>action_image, archery_y
    deactivate Archery

    Note over Button,Archery: Move up request

    Button->>Archery: AR_GAME_ARCHERY_UP
    activate Archery
    Note right of Archery: Move upward:<br/>archery_y -= STEP_ARCHERY_AXIS_Y<br/>Clamp to AXIS_Y_ARCHERY_MIN
    deactivate Archery

    Timer->>Screen: AR_GAME_TIME_TICK
    activate Screen
    Screen->>Archery: AR_GAME_ARCHERY_UPDATE
    deactivate Screen
    activate Archery
    Note right of Archery: Update rendered position:<br/>archery.y = archery_y
    deactivate Archery

    Note over Button,Archery: Move down request

    Button->>Archery: AR_GAME_ARCHERY_DOWN
    activate Archery
    Note right of Archery: Move downward:<br/>archery_y += STEP_ARCHERY_AXIS_Y<br/>Clamp to AXIS_Y_ARCHERY_MAX
    deactivate Archery

    Timer->>Screen: AR_GAME_TIME_TICK
    activate Screen
    Screen->>Archery: AR_GAME_ARCHERY_UPDATE
    deactivate Screen
    activate Archery
    Note right of Archery: Update rendered position:<br/>archery.y = archery_y
    deactivate Archery

    Note over Screen,Archery: Game reset hides Archery

    Screen->>Archery: AR_GAME_ARCHERY_RESET
    activate Archery
    Note right of Archery: Hide Archery object:<br/>archery.visible = BLACK
    deactivate Archery
```

## III. Arrow Object Sequence

Arrow receives shoot input from the MODE button. The screen task posts `AR_GAME_ARROW_RUN` on every game tick so visible arrows keep moving to the right. When an arrow exits the screen, it is hidden, reset, and the available arrow count is restored.

**Note:** *Because a slow arrow speed would significantly affect the gameplay experience, the arrow speed setting has been locked.*

```mermaid
sequenceDiagram
    autonumber
    participant Button as Button Callback
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(Archery Game Screen)
    participant Arrow as Arrow Task
    participant Archery as Archery State
    participant Buzzer as Buzzer

    Note over Screen,Arrow: Game entry initializes Arrow state

    Screen->>Arrow: AR_GAME_ARROW_SETUP
    activate Arrow
    Note right of Arrow: Clear all arrow slots:<br/>x = 0, y = 0,<br/>visible = BLACK
    deactivate Arrow

    Note over Button,Arrow: MODE button shoots an arrow

    Button->>Arrow: AR_GAME_ARROW_SHOOT
    activate Arrow
    alt settingsetup.num_arrow == 0
        Arrow->>Buzzer: BUZZER_SOUND_3BEEP
    else arrow is available
        Note right of Arrow: Find hidden arrow slot<br/>Show arrow at archery.y - 5<br/>Decrease num_arrow
        Arrow->>Archery: Set empty bow image<br/>when no arrow remains
        Arrow->>Buzzer: BUZZER_SOUND_CLICK
    end
    deactivate Arrow

    Timer->>Screen: AR_GAME_TIME_TICK
    activate Screen
    Screen->>Arrow: AR_GAME_ARROW_RUN
    deactivate Screen
    activate Arrow
    Note right of Arrow: Move visible arrows:<br/>x += settingsetup.arrow_speed

    alt arrow reaches screen edge
        Arrow->>Arrow: 
        Note left of Archery: Hide arrow and reset x <br/>Increase num_arrow
    end
    deactivate Arrow

    Note over Screen,Arrow: Game reset hides all arrows

    Screen->>Arrow: AR_GAME_ARROW_RESET
    activate Arrow
    Note right of Arrow: Hide all arrows:<br/>visible = BLACK
    deactivate Arrow
```

## IV. Meteoroid Object Sequence

Meteoroid moves from right to left. On each tick, the screen task first posts `AR_GAME_METEOROID_RUN` to move visible meteoroids and advance their animation frame, then posts `AR_GAME_METEOROID_DETONATOR` to check arrow collisions.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(Archery Game Screen)
    participant Meteoroid as Meteoroid Task
    participant Arrow as Arrow State
    participant Bang as Bang State
    participant Buzzer as Buzzer

    Note over Screen,Meteoroid: Game entry initializes Meteoroid lanes

    Screen->>Meteoroid: AR_GAME_METEOROID_SETUP
    activate Meteoroid
    Note right of Meteoroid: Create meteoroids by lane<br/>Randomize x and action_image<br/>visible = WHITE
    deactivate Meteoroid

    Timer->>Screen: AR_GAME_TIME_TICK
    activate Screen
    Screen->>Meteoroid: AR_GAME_METEOROID_RUN
    Screen->>Meteoroid: AR_GAME_METEOROID_DETONATOR
    deactivate Screen

    activate Meteoroid
    Note right of Meteoroid: Move visible meteoroids left:<br/>x -= meteoroid_speed<br/>Advance action_image
    Meteoroid-->>Arrow: Check visible arrows<br/>against visible meteoroids

    alt arrow collides with meteoroid
        Note right of Meteoroid: Hide meteoroid<br/>Randomize next spawn
        Meteoroid->>Arrow: Hide arrow and reset position
        Note right of Arrow:Restore one arrow<br/>Add 10 score
        Meteoroid->>Bang: Show bang at collision position
        Meteoroid->>Buzzer: BUZZER_SOUND_BANG
    else no collision
        Note right of Meteoroid: Keep meteoroid moving
    end
    deactivate Meteoroid

    Note over Screen,Meteoroid: Game reset hides all meteoroids

    Screen->>Meteoroid: AR_GAME_METEOROID_RESET
    activate Meteoroid
    Note right of Meteoroid: Hide all meteoroids:<br/>visible = BLACK
    deactivate Meteoroid
```

## V. Bang Object Sequence

Bang is the explosion effect. It becomes visible when Meteoroid detects a collision. On every game tick, the screen task posts `AR_GAME_BANG_UPDATE`; visible bang effects advance their animation frame, then hide themselves and restore the matching meteoroid when the animation ends.

```mermaid
sequenceDiagram
    autonumber
    participant Meteoroid as Meteoroid Task
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(Archery Game Screen)
    participant Bang as Bang Task

    Note over Screen,Bang: Game entry initializes Bang effects

    Screen->>Bang: AR_GAME_BANG_SETUP
    activate Bang
    Note right of Bang: Clear bang effects:<br/>visible = BLACK<br/>action_image = first frame
    deactivate Bang

    Meteoroid->>Bang: Collision creates visible bang
    activate Bang
    Note right of Bang: Set bang[i].visible = WHITE<br/>Set x, y, action_image
    deactivate Bang

    alt bang is not visible
        Timer->>Screen: AR_GAME_TIME_TICK
        activate Screen
        Screen->>Bang: AR_GAME_BANG_UPDATE
        deactivate Screen
        activate Bang
        Note right of Bang: No visible explosion<br/>No frame is advanced
        deactivate Bang
    else bang is visible
        Timer->>Screen: AR_GAME_TIME_TICK
        activate Screen
        Screen->>Bang: AR_GAME_BANG_UPDATE
        deactivate Screen
        activate Bang
        Note right of Bang: Advance explosion animation frame<br/>Repeat on each game tick

        alt animation reaches end
            Note right of Bang: Hide bang and reset frame
            Bang->>Meteoroid: Restore matching meteoroid
        end
        deactivate Bang
    end

    Note over Screen,Bang: Game reset hides all bang effects

    Screen->>Bang: AR_GAME_BANG_RESET
    activate Bang
    Note right of Bang: Hide all bang effects:<br/>visible = BLACK
    deactivate Bang
```

## VI. Border Object Sequence

Border protects the safe area. Each game tick asks Border to check level-up progress and game-over conditions. If the score reaches the next threshold, Border increases meteoroid speed. If a visible meteoroid reaches the border, Border posts `AR_GAME_RESET` back to the screen task.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(Archery Game Screen)
    participant Border as Border Task
    participant Meteoroid as Meteoroid State

    Note over Screen,Border: Game entry initializes Border state

    Screen->>Timer: timer_set(AR_GAME_TIME_TICK)
    Screen->>Border: AR_GAME_BORDER_SETUP
    activate Border
    Note right of Border: Set border x, visible,<br/>and action_image
    deactivate Border

    alt score below threshold and meteoroid safe
        Timer->>Screen: AR_GAME_TIME_TICK
        activate Screen
        Screen->>Border: AR_GAME_LEVEL_UP
        Screen->>Border: AR_GAME_CHECK_GAME_OVER
        deactivate Screen
        activate Border
        Border-->>Meteoroid: Check visible meteoroid x
        Note right of Border: No level-up<br/>No game-over reset
        deactivate Border
    else score reaches next level threshold
        Timer->>Screen: AR_GAME_TIME_TICK
        activate Screen
        Screen->>Border: AR_GAME_LEVEL_UP
        Screen->>Border: AR_GAME_CHECK_GAME_OVER
        deactivate Screen
        activate Border
        Note right of Border: Increase meteoroid_speed<br/>Move next level score threshold
        Border-->>Meteoroid: Check visible meteoroid x
        deactivate Border
    else visible meteoroid reaches border
        Timer->>Screen: AR_GAME_TIME_TICK
        activate Screen
        Screen->>Border: AR_GAME_LEVEL_UP
        Screen->>Border: AR_GAME_CHECK_GAME_OVER
        deactivate Screen
        activate Border
        Border-->>Meteoroid: Check visible meteoroid x
        Border->>Screen: AR_GAME_RESET
        Note right of Border: Game-over condition met
        deactivate Border
    end

    Note over Screen,Border: Game reset hides Border

    Screen->>Border: AR_GAME_BORDER_RESET
    activate Border
    Note right of Border: Hide Border object:<br/>visible = BLACK
    deactivate Border
```

## VII. Code References

| Object | Source file | Header file |
|---|---|---|
| Archery | `application/sources/app/game/archery_game/ar_game_archery.cpp` | `application/sources/app/game/archery_game/ar_game_archery.h` |
| Arrow | `application/sources/app/game/archery_game/ar_game_arrow.cpp` | `application/sources/app/game/archery_game/ar_game_arrow.h` |
| Meteoroid | `application/sources/app/game/archery_game/ar_game_meteoroid.cpp` | `application/sources/app/game/archery_game/ar_game_meteoroid.h` |
| Bang | `application/sources/app/game/archery_game/ar_game_bang.cpp` | `application/sources/app/game/archery_game/ar_game_bang.h` |
| Border | `application/sources/app/game/archery_game/ar_game_border.cpp` | `application/sources/app/game/archery_game/ar_game_border.h` |
