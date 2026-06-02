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

Gunner owns the player position. The screen task initializes the Gunner object when gameplay starts, then the periodic game tick translates the latched direction (`gunner_dir`) into `ZW_GAME_GUNNER_UP` / `ZW_GAME_GUNNER_DOWN` and always posts `ZW_GAME_GUNNER_UPDATE`. Button callbacks only update `gunner_dir` inside the screen task; they do not post to the Gunner task directly. Movement changes the internal `gunner_y` value, clamps it (`AXIS_Y_GUNNER_MIN`..`AXIS_Y_GUNNER_MAX`), then copies it into the rendered `gunner.y`. `ZW_GAME_GUNNER_UPDATE` also clears the recoil frame by resetting `gunner.action_image` from `2` back to `1` (the recoil frame is raised by the Bullet task on `ZW_GAME_BULLET_SHOOT`).

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_gunner_sequence.png" alt="Gunner game sequence logic" width="720"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> Gunner sequence logic</p>

## III. Bullet Object Sequence

Bullet receives shoot input from the MODE button (only while `zw_game_state == GAME_PLAY`). `ZW_GAME_BULLET_SHOOT` picks the first free slot in `bullet[]`, spawns it at `(gunner.x + 15, gunner.y - 8)`, sets `gunner.action_image = 2` to show the recoil frame, and plays `BUZZER_SOUND_CLICK`. The screen task posts `ZW_GAME_BULLET_RUN` on every game tick so visible bullets keep moving to the right by `STEP_BULLET_AXIS_X`. When a bullet reaches `MAX_AXIS_X_BULLET`, it is hidden and its x position is cleared.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_bullet_sequence.png" alt="Bullet game sequence logic" width="720"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 2:</em></strong> Bullet sequence logic</p>

## IV. Zombie Object Sequence

Zombie moves from right to left with zigzag motion on the Y axis. On each tick, the screen task posts `ZW_GAME_ZOMBIE_RUN` to move visible zombies (clamped at `-ZOMBIE_MIN_LEFT_OFFSET` on the left) and advance their animation frame, then posts `ZW_GAME_ZOMBIE_DETONATOR` to check bullet collisions. Zombies that are still `rising` from a tombstone do not move horizontally — they walk up by 1 px per tick for `ZOMBIE_RISE_TICKS` ticks, then join the normal flow. The task also auto-respawns from the right edge to keep at least `NUM_ZOMBIES_INIT` zombies alive at all times.

`ZW_GAME_ZOMBIE_DETONATOR` scans every visible non-rising zombie against every visible bullet using an AABB check (bullet must have passed 12 px into the zombie body). On a hit, the bullet slot is freed, the zombie slot is freed, a Bang is spawned in the first free `bang[]` slot at `(zombie.x + 5, zombie.y - 2)`, `zw_game_score` is increased by 10, and `BUZZER_SOUND_BANG` is played. Rising zombies are immune to bullets.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(scr_game_zomwar)
    participant Zombie as Zombie Task
    participant Bullet as Bullet State
    participant Bang as Bang State
    participant Buzzer as Buzzer

    Note over Screen,Zombie: Game entry initializes Zombie state

    Screen->>Zombie: ZW_GAME_ZOMBIE_SETUP
    activate Zombie
    Note right of Zombie: game_active = true<br/>zw_game_zombie_speed = settingdata.zombie_speed<br/>For all NUM_ZOMBIES: visible = BLACK<br/>For first NUM_ZOMBIES_INIT:<br/>x = rand()%39 + 130<br/>y = rand()%(ZOMBIE_Y_MAX - ZOMBIE_Y_MIN + 1) + ZOMBIE_Y_MIN<br/>visible = WHITE, action_image = rand()%3 + 1<br/>dy = 0, zigzag_timer = rand()%10 + 5<br/>rising = false, rise_ticks = 0
    deactivate Zombie

    Timer->>Screen: ZW_GAME_TIME_TICK
    activate Screen
    Screen->>Zombie: ZW_GAME_ZOMBIE_RUN
    Screen->>Zombie: ZW_GAME_ZOMBIE_DETONATOR
    deactivate Screen

    activate Zombie
    Note right of Zombie: ZW_GAME_ZOMBIE_RUN
    alt !game_active
        Note right of Zombie: return
    end
    Note right of Zombie: alive = 0<br/>For each visible zombie i:
    alt zombie[i].rising
        alt rise_ticks > 0
            Note right of Zombie: zombie.y--<br/>rise_ticks--
        else rise_ticks == 0
            Note right of Zombie: rising = false<br/>zigzag_timer = rand()%10 + 5
        end
        Note right of Zombie: action_image cycles 1->2->3->1<br/>continue (no horizontal move)
    else not rising
        alt x - speed < -ZOMBIE_MIN_LEFT_OFFSET
            Note right of Zombie: zombie.x = -ZOMBIE_MIN_LEFT_OFFSET
        else
            Note right of Zombie: zombie.x -= zw_game_zombie_speed
        end
        alt zigzag_timer > 0
            Note right of Zombie: zigzag_timer--
        else
            Note right of Zombie: dy = (rand()%3) - 1<br/>zigzag_timer = rand()%10 + 5
        end
        Note right of Zombie: new_y = y + dy clamped to [ZOMBIE_Y_MIN, ZOMBIE_Y_MAX]<br/>(dy = 0 if clamped)<br/>action_image cycles 1->2->3->1
    end
    Note right of Zombie: Auto-respawn while alive < NUM_ZOMBIES_INIT:<br/>fill free slot with random x/y/action_image/zigzag_timer
    deactivate Zombie

    activate Zombie
    Note right of Zombie: ZW_GAME_ZOMBIE_DETONATOR<br/>For each visible non-rising zombie i,<br/>for each visible bullet j:
    alt AABB hit (bullet has passed 12 px into zombie body)
        Note right of Zombie: bullet[j].visible = BLACK, bullet[j].x = 0
        Zombie->>Bang: First free slot:<br/>visible = WHITE<br/>x = zombie.x + 5, y = zombie.y - 2<br/>action_image = 1
        Note right of Zombie: zw_game_score += 10
        Zombie->>Buzzer: BUZZER_SOUND_BANG
        Note right of Zombie: zombie[i].visible = BLACK<br/>break inner loop
    end
    deactivate Zombie

    Note over Screen,Zombie: Game reset hides all zombies

    Screen->>Zombie: ZW_GAME_ZOMBIE_RESET
    activate Zombie
    Note right of Zombie: game_active = false<br/>For all NUM_ZOMBIES: visible = BLACK
    deactivate Zombie

    Note over Screen,Zombie: Menu screen reuses zombie[0] only

    Screen->>Zombie: ZW_GAME_ZOMBIE_SETUP_MENU
    activate Zombie
    Note right of Zombie: game_active = true<br/>All visible = BLACK<br/>zombie[0]: x = LCD_WIDTH + 3, y = AXIS_Y_GUNNER - 10<br/>visible = WHITE, action_image = 1, rising = false
    deactivate Zombie

    Screen->>Zombie: ZW_GAME_ZOMBIE_RUN_MENU
    activate Zombie
    Note right of Zombie: zombie[0].x -= MENU_ZOMBIE_SPEED<br/>action_image cycles 1->2->3->1
    alt zombie[0].x < -SIZE_BITMAP_ZOMBIES_X
        Note right of Zombie: Wrap back to x = LCD_WIDTH + 3<br/>y = AXIS_Y_GUNNER - 10
    end
    alt AABB hit vs any visible bullet
        Note right of Zombie: bullet freed<br/>spawn Bang at (x+5, y-2) (no score, no sound)<br/>zombie[0] wraps to right edge
    end
    deactivate Zombie
```

The Zombie task also exposes `ZW_GAME_ZOMBIE_SETUP_MENU` and `ZW_GAME_ZOMBIE_RUN_MENU` signals used by the menu screen to display a single demo zombie that loops across the screen and reacts to bullets without affecting the score.

## V. Bang Object Sequence

Bang is the explosion effect. It is not spawned by a dedicated signal — the Zombie task and Car task directly mutate the `bang[]` array when they detect a collision. On every game tick the screen task posts `ZW_GAME_BANG_UPDATE`; each visible bang advances its animation frame, and when the frame counter rolls past 3 the bang hides itself and resets its frame to 1.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer<br/>(100 ms)
    participant Screen as Display Task<br/>(scr_game_zomwar)
    participant Zombie as Zombie Task
    participant Car as Car Task
    participant Bang as Bang Task

    Note over Screen,Bang: Game entry initializes Bang state

    Screen->>Bang: ZW_GAME_BANG_SETUP
    activate Bang
    Note right of Bang: For each i in [0..NUM_BANG):<br/>bang[i].visible = BLACK<br/>bang[i].action_image = 1
    deactivate Bang

    Note over Zombie,Bang: Bang is spawned by direct array mutation,<br/>not via a signal

    Zombie-->>Bang: On bullet/zombie hit (ZW_GAME_ZOMBIE_DETONATOR):<br/>first free slot -> visible = WHITE,<br/>x = zombie.x + 5, y = zombie.y - 2,<br/>action_image = 1
    Car-->>Bang: On car/zombie hit (ZW_GAME_CAR_RUN):<br/>first free slot -> visible = WHITE,<br/>x = zombie.x + 5, y = zombie.y - 2,<br/>action_image = 1

    Timer->>Screen: ZW_GAME_TIME_TICK
    activate Screen
    Screen->>Bang: ZW_GAME_BANG_UPDATE
    deactivate Screen
    activate Bang
    Note right of Bang: For each i in [0..NUM_BANG):
    alt bang[i].visible == WHITE
        Note right of Bang: bang[i].action_image++
    end
    alt bang[i].action_image > 3
        Note right of Bang: bang[i].action_image = 1<br/>bang[i].visible = BLACK
    end
    deactivate Bang

    Note over Screen,Bang: Game reset clears all bangs

    Screen->>Bang: ZW_GAME_BANG_RESET
    activate Bang
    Note right of Bang: For each i in [0..NUM_BANG):<br/>bang[i].visible = BLACK<br/>bang[i].action_image = 1
    deactivate Bang
```

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
    Border-->>Car: lane = (zombie.y - ZOMBIE_Y_MIN) / 10<br/>if (lane >= NUM_LANES) lane = NUM_LANES - 1

    alt !car[lane].visible
        Border->>Screen: ZW_GAME_RESET (post to AC_TASK_DISPLAY_ID)<br/>break out of loop
    else car protects the lane
        Note right of Border: No game-over for this zombie
    end

    alt !wave_warning_active && zw_game_score >= wave_last_score + WAVE_SCORE_INTERVAL
        Note right of Border: wave_warning_active = true<br/>wave_warning_timer = WARNING_BLINK_DURATION
    end
    alt wave_warning_active
        alt wave_warning_timer > 0
            Note right of Border: wave_warning_timer--
        else wave_warning_timer == 0
            Note right of Border: wave_warning_active = false<br/>wave_last_score += WAVE_SCORE_INTERVAL<br/>wave_level++<br/>if zw_game_zombie_speed < ZOMBIE_SPEED_MAX:<br/>zw_game_zombie_speed++<br/>Spawn up to WAVE_SPAWN_COUNT zombies into free slots
        end
    end
    deactivate Border

    Note over Screen,Border: Game reset hides Border

    Screen->>Border: ZW_GAME_BORDER_RESET
    activate Border
    Note right of Border: border.x = AXIS_X_BORDER<br/>border.visible = BLACK<br/>zw_game_score = 0<br/>wave_last_score = 0<br/>wave_warning_timer = 0<br/>wave_warning_active = false<br/>wave_level = 0
    deactivate Border
```

## VII. Car Object Sequence

Car implements the lawnmower-style rescue. There is one car slot per lane (`NUM_LANES = 5`); whether a slot is initially visible is decided by the `settingdata.num_car` bitmask. On each tick the screen task posts `ZW_GAME_CAR_RUN`. The handler runs two loops in order. **Loop 1** scans every visible zombie: if a zombie has reached the left edge (`x <= -ZOMBIE_MIN_LEFT_OFFSET`), `find_nearest_mower()` picks the parked car (`visible && !running`) whose `y` is closest to the zombie within `CAR_HIT_RANGE_Y`; if one is found, that car starts running and the zombie is hidden (rescued), otherwise the zombie is left at the edge for Border to detect a game over. For zombies not at the left edge, the loop activates a parked car whose bumper has been reached (`zombie.x + ZOMBIE_MIN_LEFT_OFFSET <= car.x + SIZE_BITMAP_CAR_X` and within `CAR_HIT_RANGE_Y`). **Loop 2** advances every running car by `CAR_SPEED`, cycles `action_image` through 1→2→3→1, and mows down zombies in the same lane (creates a Bang at `(zombie.x + 5, zombie.y - 2)`, +10 score, plays `BUZZER_SOUND_BANG`, hides the zombie). When a running car leaves the screen (`car.x > LCD_WIDTH`) its slot is marked `visible = false` and `running = false`.

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
    Note right of Car: Loop 1: scan visible zombies
    Car-->>Zombie: For each visible zombie i
    alt zombie[i].x <= -ZOMBIE_MIN_LEFT_OFFSET
        Note right of Car: find_nearest_mower(zombie.y):<br/>pick car with visible && !running<br/>minimising |zombie.y - car.y|, must be <= CAR_HIT_RANGE_Y
        alt mower found (m >= 0)
            Note right of Car: car[m].running = true<br/>zombie[i].visible = BLACK (rescued)
        else no parked car in range
            Note right of Car: Leave zombie at edge<br/>Border will trigger ZW_GAME_RESET
        end
        Note right of Car: continue to next zombie
    else zombie not at left edge
        Note right of Car: For each car m with visible && !running:<br/>if |zombie.y - car.y| <= CAR_HIT_RANGE_Y<br/>and zombie.x + ZOMBIE_MIN_LEFT_OFFSET<br/>     <= car.x + SIZE_BITMAP_CAR_X<br/>then car[m].running = true
    end

    Note right of Car: Loop 2: for each car with visible && running<br/>car.x += CAR_SPEED<br/>action_image cycles 1->2->3->1
    Car-->>Zombie: For each visible zombie j with<br/>|zombie.y - car.y| <= CAR_HIT_RANGE_Y
    alt zombie.x + ZOMBIE_MIN_LEFT_OFFSET <= car.x + SIZE_BITMAP_CAR_X
        Car->>Bang: First free bang slot:<br/>x = zombie.x + 5, y = zombie.y - 2<br/>action_image = 1, visible = WHITE
        Note right of Car: zw_game_score += 10<br/>zombie[j].visible = BLACK
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
