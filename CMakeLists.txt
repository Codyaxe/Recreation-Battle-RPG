cmake_minimum_required(VERSION 3.30)
project(Battle_RPG)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Battle_RPG
    action.cpp
    action.h
    attacks.cpp
    attacks.h
    buffs.cpp
    buffs.h
    character.cpp
    character.h
    components.cpp
    components.h
    debuffs.cpp
    debuffs.h
    effects.cpp
    effects.h
    items.cpp
    items.h
    main.cpp
    observer.cpp
    observer.h
    return_flags.h
    spells.cpp
    spells.h
    status.cpp
    status.h
    summons.cpp
    summons.h
    technical.cpp
    technical.h
    test.cpp
    traits.cpp
    traits.h)
