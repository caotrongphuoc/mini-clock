# Game Object Sequences

This document describes the runtime sequence of each main object in Zomwar. Each object is handled by its own AK task and receives signals from the screen task (`scr_game_zomwar`), button callbacks, the periodic game tick timer, or other object tasks.

## I. Object Summary

| Object | Task ID | Handler | Main responsibility |
|---|---|---|---|
| Gunner | `ZW_GAME_GUNNER_ID` | `zw_game_gunner_handle()` | Controls the player position and gunner image state. |
| Bullet | `ZW_GAME_BULLET_ID` | `zw_game_bullet_handle()` | Shoots bullets, moves active bullets, and hides them when they exit the screen. |
| Zombie | `ZW_GAME_ZOMBIE_ID` | `zw_game_zombie_handle()` | Spawns zombies, moves them (zigzag, rising from tombstones), checks collision with bullets, and updates score. |
| Bang | `ZW_GAME_BANG_ID` | `zw_game_bang_handle()` | Plays explosion animation after a zombie is hit. |
| Border | `ZW_GAME_BORDER_ID` | `zw_game_border_handle()` | Checks wave level-up and game-over conditions. |
| Car | `ZW_GAME_CAR_ID` | `zw_game_car_handle()` | Lawnmower-style rescue cars that activate when a zombie reaches the left edge or touches a parked car. |
| Tombstone | `ZW_GAME_TOMBSTONE_ID` | `zw_game_tombstone_handle()` | Spawns extra zombies that rise out of active tombstones. |

The screen task posts `ZW_GAME_TIME_TICK` every `ZW_GAME_TIME_TICK_INTERVAL` (100 ms). On each tick the screen task fans out signals to every object task in a fixed order.

## II. Gunner Object Sequence

Gunner owns the player position. The screen task initializes the Gunner object when gameplay starts, then the periodic game tick translates the latched direction (`gunner_dir`) into `ZW_GAME_GUNNER_UP` / `ZW_GAME_GUNNER_DOWN` and always posts `ZW_GAME_GUNNER_UPDATE`. Button callbacks only update `gunner_dir` inside the screen task; they do not post to the Gunner task directly. Movement changes the internal `gunner_y` value, clamps it, then copies it into the rendered `gunner.y`.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_gunner_sequence.png" alt="gunner sequence logic" width="720"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> Gunner sequence logic</p>

## III. Bullet Object Sequence

Bullet receives shoot input from the MODE button (only while `zw_game_state == GAME_PLAY`). The screen task posts `ZW_GAME_BULLET_RUN` on every game tick so visible bullets keep moving to the right. When a bullet exits the screen, it is hidden and its x position is cleared.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_bullet_sequence.png" alt="bullet sequence logic" width="720"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 2:</em></strong> Bullet sequence logic</p>

## IV. Zombie Object Sequence

Zombie moves from right to left with zigzag motion on the Y axis. On each tick, the screen task posts `ZW_GAME_ZOMBIE_RUN` to move visible zombies and advance their animation frame, then posts `ZW_GAME_ZOMBIE_DETONATOR` to check bullet collisions. Zombies that are still `rising` from a tombstone do not move horizontally — they walk up by 1 px per tick for `ZOMBIE_RISE_TICKS` ticks, then join the normal flow. The task also auto-respawns from the right edge to keep at least `NUM_ZOMBIES_INIT` zombies alive at all times.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_zombie_sequence.png" alt="bullet sequence logic" width="720"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 3:</em></strong> Zombie sequence logic</p>

The Zombie task also exposes `ZW_GAME_ZOMBIE_SETUP_MENU` and `ZW_GAME_ZOMBIE_RUN_MENU` signals used by the menu screen to display a single demo zombie that loops across the screen and reacts to bullets without affecting the score.

## V. Bang Object Sequence

Bang is the explosion effect. It is not spawned by a dedicated signal — the Zombie task and Car task directly mutate the `bang[]` array when they detect a collision. On every game tick the screen task posts `ZW_GAME_BANG_UPDATE`; each visible bang advances its animation frame, and when the frame counter rolls past 3 the bang hides itself and resets its frame to 1.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_bang_sequence.png" alt="bullet sequence logic" width="720"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 4:</em></strong> Zombie sequence logic</p>

## VI. Border Object Sequence

Border manages wave progression and the game-over check. Each game tick the screen task posts both `ZW_GAME_CHECK_GAME_OVER` and `ZW_GAME_LEVEL_UP` to the Border task. If a visible zombie reaches the left edge in a lane that has no visible car, Border posts `ZW_GAME_RESET` to the display task. When `zw_game_score` reaches the next `WAVE_SCORE_INTERVAL` threshold, Border starts a warning blink for `WARNING_BLINK_DURATION` ticks, then increases `zw_game_zombie_speed` (capped at `ZOMBIE_SPEED_MAX`), increments `wave_level`, and spawns `WAVE_SPAWN_COUNT` new zombies.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(scr_game_zomwar)
    participant Border as Border Task
    participant Zombie as Zombie State
    participant Car as Car State

    Note over Screen,Border: Game entry initializes Border state

    Screen->>Border: ZW_GAME_BORDER_SETUP
    activate Border
    Note right of Border: border.x = AXIS_X_BORDER<br/>border.visible = WHITE<br/>border.action_image = 0<br/>wave_last_score = 0<br/>wave_warning_timer = 0<br/>wave_warning_active = false<br/>wave_level = 0
    deactivate Border

    Timer->>Screen: ZW_GAME_TIME_TICK
    activate Screen
    Screen->>Border: ZW_GAME_CHECK_GAME_OVER
    Screen->>Border: ZW_GAME_LEVEL_UP
    deactivate Screen

    activate Border
    Border-->>Zombie: For each visible zombie at x <= -ZOMBIE_MIN_LEFT_OFFSET
    Border-->>Car: Compute lane = (zombie.y - ZOMBIE_Y_MIN) / 10

    alt no car visible in that lane
        Border->>Screen: ZW_GAME_RESET (post to AC_TASK_DISPLAY_ID)
    else car protects the lane
        Note right of Border: No game-over
    end

    alt zw_game_score >= wave_last_score + WAVE_SCORE_INTERVAL
        Note right of Border: wave_warning_active = true<br/>wave_warning_timer = WARNING_BLINK_DURATION
    end
    alt wave_warning_active && warning_timer > 0
        Note right of Border: wave_warning_timer--
    else wave_warning_active && timer == 0
        Note right of Border: wave_warning_active = false<br/>wave_last_score += WAVE_SCORE_INTERVAL<br/>wave_level++<br/>If zw_game_zombie_speed < ZOMBIE_SPEED_MAX:<br/>zw_game_zombie_speed++<br/>Spawn WAVE_SPAWN_COUNT zombies
    end
    deactivate Border

    Note over Screen,Border: Game reset hides Border

    Screen->>Border: ZW_GAME_BORDER_RESET
    activate Border
    Note right of Border: border.x = AXIS_X_BORDER<br/>border.visible = BLACK<br/>zw_game_score = 0<br/>wave_last_score = 0<br/>wave_warning_timer = 0<br/>wave_warning_active = false<br/>wave_level = 0
    deactivate Border
```

## VII. Car Object Sequence

Car implements the lawnmower-style rescue. There is one car slot per lane (`NUM_LANES = 5`); whether a slot is initially visible is decided by the `settingdata.num_car` bitmask. On each tick the screen task posts `ZW_GAME_CAR_RUN`. The handler runs two loops in order: first it scans zombies — if any visible zombie has touched the left edge it activates the nearest free parked car in range (`CAR_HIT_RANGE_Y`) and kills that zombie; it also activates a parked car when a zombie walks into its bumper. Then it advances every running car to the right, mowing down zombies in the same lane (creating a Bang, +10 score, `BUZZER_SOUND_BANG`). When a running car leaves the screen its slot becomes invisible and consumed.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(scr_game_zomwar)
    participant Car as Car Task
    participant Zombie as Zombie State
    participant Bang as Bang State
    participant Buzzer as Buzzer

    Note over Screen,Car: Game entry initializes Car state

    Screen->>Car: ZW_GAME_CAR_SETUP
    activate Car
    Note right of Car: game_active = true<br/>For each lane i:<br/>car[i].x = AXIS_X_CAR<br/>car[i].y = lane_y[i]<br/>car[i].lane = i<br/>car[i].visible = (settingdata.num_car >> i) & 1<br/>car[i].running = false<br/>car[i].action_image = 1
    deactivate Car

    Timer->>Screen: ZW_GAME_TIME_TICK
    activate Screen
    Screen->>Car: ZW_GAME_CAR_RUN
    deactivate Screen

    activate Car
    Car-->>Zombie: Scan visible zombies (loop 1)
    alt zombie at left edge
        Note right of Car: Find nearest parked car (running=false)<br/>within CAR_HIT_RANGE_Y
        alt car found
            Note right of Car: car[m].running = true<br/>Hide zombie (rescued)
        else no parked car
            Note right of Car: Leave zombie at edge<br/>Border may trigger ZW_GAME_RESET
        end
    else zombie touches parked car
        Note right of Car: car[m].running = true
    end

    Note right of Car: Loop 2: each running car<br/>x += CAR_SPEED<br/>Advance action_image (1..3)
    Car-->>Zombie: For zombies in same lane within CAR_HIT_RANGE_Y
    alt zombie x reaches car front
        Car->>Bang: Spawn bang at (zombie.x + 5, zombie.y - 2)
        Note right of Car: zw_game_score += 10<br/>Hide zombie
        Car->>Buzzer: BUZZER_SOUND_BANG
    end
    alt car[i].x > LCD_WIDTH
        Note right of Car: car[i].visible = false<br/>car[i].running = false
    end
    deactivate Car

    Note over Screen,Car: Game reset hides all cars

    Screen->>Car: ZW_GAME_CAR_RESET
    activate Car
    Note right of Car: game_active = false<br/>For each lane i:<br/>car[i].x = AXIS_X_CAR<br/>car[i].y = lane_y[i]<br/>car[i].visible = false<br/>car[i].running = false
    deactivate Car
```

## VIII. Tombstone Object Sequence

Tombstone produces extra zombies that rise out of decorative tombstones. `NUM_TOMBSTONES = 10` slots, laid out as `TOMBSTONES_PER_LANE = 2` per lane. Whether each slot is active is decided by the `settingdata.tombstone_lane_1` and `settingdata.tombstone_lane_2` bitmasks. On every tick the screen task posts `ZW_GAME_TOMBSTONE_SPAWN`; the handler decrements a spawn timer and only acts every `TOMBSTONE_SPAWN_INTERVAL` ticks. When it fires, it picks a random tombstone — if active, it inserts a `rising` zombie into the first free zombie slot.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(scr_game_zomwar)
    participant Tombstone as Tombstone Task
    participant Zombie as Zombie State

    Note over Screen,Tombstone: Game entry initializes Tombstone state

    Screen->>Tombstone: ZW_GAME_TOMBSTONE_SETUP
    activate Tombstone
    Note right of Tombstone: tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL<br/>For each lane l:<br/>tombstones[l].x = rand()%20 + 65<br/>tombstones[l].lane = l<br/>tombstones[l].active = (settingdata.tombstone_lane_1 >> l) & 1<br/>tombstones[l+NUM_LANES].x = rand()%20 + 90<br/>tombstones[l+NUM_LANES].lane = l<br/>tombstones[l+NUM_LANES].active = (settingdata.tombstone_lane_2 >> l) & 1
    deactivate Tombstone

    Timer->>Screen: ZW_GAME_TIME_TICK
    activate Screen
    Screen->>Tombstone: ZW_GAME_TOMBSTONE_SPAWN
    deactivate Screen
    activate Tombstone
    alt tombstone_spawn_timer > 0
        Note right of Tombstone: tombstone_spawn_timer--<br/>(no spawn this tick)
    else timer reaches 0
        Note right of Tombstone: tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL<br/>tidx = rand() % NUM_TOMBSTONES
        alt tombstones[tidx].active
            Tombstone->>Zombie: Use first free slot:<br/>x = tombstones[tidx].x<br/>y = lane_y[lane] + SIZE_BITMAP_TOMBSTONE_Y<br/>visible = WHITE<br/>action_image = 1<br/>dy = 0, zigzag_timer = 0<br/>rising = true<br/>rise_ticks = ZOMBIE_RISE_TICKS
        else inactive tombstone
            Note right of Tombstone: Skip spawn
        end
    end
    deactivate Tombstone

    Note over Screen,Tombstone: Game reset clears all tombstones

    Screen->>Tombstone: ZW_GAME_TOMBSTONE_RESET
    activate Tombstone
    Note right of Tombstone: tombstone_spawn_timer = 0<br/>For every slot:<br/>x = 0, lane = 0, active = false
    deactivate Tombstone
```

## IX. Per-Tick Signal Order

The screen task `scr_game_zomwar` posts the following sequence on every `ZW_GAME_TIME_TICK`:

1. `ZW_GAME_GUNNER_UP` or `ZW_GAME_GUNNER_DOWN` (depending on `gunner_dir`)
2. `ZW_GAME_GUNNER_UPDATE`
3. `ZW_GAME_BULLET_RUN`
4. `ZW_GAME_ZOMBIE_RUN`
5. `ZW_GAME_ZOMBIE_DETONATOR`
6. `ZW_GAME_TOMBSTONE_SPAWN`
7. `ZW_GAME_CAR_RUN`
8. `ZW_GAME_BANG_UPDATE`
9. `ZW_GAME_CHECK_GAME_OVER`
10. `ZW_GAME_LEVEL_UP`

On `SCREEN_ENTRY` the screen task posts the matching `*_SETUP` signals to all object tasks and starts the periodic `ZW_GAME_TIME_TICK` timer. On `ZW_GAME_RESET` it removes the timer, posts the `*_RESET` signals, saves `gamescore.score_now = zw_game_score`, transitions to `GAME_OVER`, and arms a one-shot `ZW_GAME_EXIT_GAME` timer.

## X. Code References

| Object | Source file | Header file |
|---|---|---|
| Gunner | `application/sources/app/game/game_zomwar/zw_game_gunner.cpp` | `application/sources/app/game/game_zomwar/zw_game_gunner.h` |
| Bullet | `application/sources/app/game/game_zomwar/zw_game_bullet.cpp` | `application/sources/app/game/game_zomwar/zw_game_bullet.h` |
| Zombie | `application/sources/app/game/game_zomwar/zw_game_zombie.cpp` | `application/sources/app/game/game_zomwar/zw_game_zombie.h` |
| Bang | `application/sources/app/game/game_zomwar/zw_game_bang.cpp` | `application/sources/app/game/game_zomwar/zw_game_bang.h` |
| Border | `application/sources/app/game/game_zomwar/zw_game_border.cpp` | `application/sources/app/game/game_zomwar/zw_game_border.h` |
| Car | `application/sources/app/game/game_zomwar/zw_game_car.cpp` | `application/sources/app/game/game_zomwar/zw_game_car.h` |
| Tombstone | `application/sources/app/game/game_zomwar/zw_game_tombstone.cpp` | `application/sources/app/game/game_zomwar/zw_game_tombstone.h` |
| Screen | `application/sources/app/screens/scr_game_zomwar.cpp` | `application/sources/app/screens/scr_game_zomwar.h` |
