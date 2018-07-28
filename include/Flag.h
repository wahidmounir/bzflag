/* bzflag
 * Copyright (c) 1993-2018 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/** @file
 * Flags add some spice to the game.  There are two kinds of flags:
 * team flags and super flags.  Super flags come in two types: good
 * and bad.
 *
 *   When playing a "capture the flag" style game, each team with at
 * least one player has a team flag which has the same color as the
 * team.  A team flag will remain in the game as long as there is a
 * player on that team.  A team flag may be picked up and freely
 * dropped at any time.  It may be captured, which causes it to go
 * back to it's home position (centered in the team base).  If a
 * flag is dropped by a hostile player in a third team's base, the
 * flag will go to the third team's flag safety position.  For example,
 * if a Green Team player dropped the Red Flag on Blue's Base, the
 * Red Flag would go to the Blue Team's safety position.  This is
 * because if it stayed in the Blue Base, any Red Team member who
 * picked it up would instantly have brought his team flag into
 * enemy territory and so blow up his whole team.
 *
 *   A super flag causes the characteristics of the tank that possesses
 * it to change.  A good super flag generally makes the tank more
 * powerful or deadly.  A bad super flag generally does the opposite.
 * A good super flag may always be dropped.  A bad super flag is
 * "sticky" which means that it can't be freely dropped.  The server
 * may have some means of getting rid of a bad super flag (perhaps
 * by destroying an enemy or two or after waiting 20 seconds).
 * The creation and destruction of super flags is under the server's
 * control so super flags may appear and disappear seemingly at
 * random.
 */

#ifndef BZF_FLAG_H
#define BZF_FLAG_H

#include "common.h"

/* system interface headers */
#include <set>
#include <map>
#include <string>
#include <vector>
#include <memory>

/* common interface headers */
#include "global.h"
#include "Address.h"

enum class FlagEffect
{
    Normal,
    Velocity,
    QuickTurn,
    OscillationOverthruster,
    RapidFire,
    MachineGun,
    GuidedMissile,
    Laser,
    Ricochet,
    SuperBullet,
    InvisibleBullet,
    Stealth,
    Tiny,
    Narrow,
    Shield,
    Steamroller,
    ShockWave,
    PhantomZone,
    Jumping,
    Identify,
    Cloaking,
    Useless,
    Masquerade,
    Seer,
    Thief,
    Burrow,
    Wings,
    Agility,
    Colorblindness,
    Obesity,
    LeftTurnOnly,
    RightTurnOnly,
    ForwardOnly,
    ReverseOnly,
    Momentum,
    Blindness,
    Jamming,
    WideAngle,
    NoJumping,
    TriggerHappy,
    ReverseControls,
    Bouncy,
    NoShot,
};

/** This enum says where a flag is. */
enum class FlagStatus
{
    /// the flag is not present in the world
    NoExist = 0,
    /// the flag is sitting on the ground and can be picked up
    OnGround,
    /// the flag is being carried by a tank
    OnTank,
    /// the flag is falling through the air
    InAir,
    /// the flag is entering the world
    Coming,
    /// the flag is leaving the world
    Going
};

/** This enum tells us if the flag type is droppable, and what happens to it
    when it's droppped. */
enum class FlagEndurance
{
    /// permanent flag
    Normal = 0,
    /// disappears after use
    Unstable = 1,
    /// can't be dropped normally
    Sticky = 2
};

/** This enum tells the "quality" of the flag type, i.e. whether it's good
    or bad */
enum class FlagQuality
{
    Good = 0,
    Bad = 1,
    Last
};

/** This enum says if the flag type gives the carrier a special shooting
    ability. */
enum ShotType
{
    Normal = 0,
    Special = 1
};

const int       FlagPLen = 55;

#define FlagPackSize 2

/** This class represents a flagtype, like "GM" or "CL". */
class FlagType
{
public:
    typedef std::shared_ptr<FlagType> Ptr;
    typedef std::map<std::string, FlagType::Ptr> TypeMap;
    typedef std::set<FlagType::Ptr> Set;

    FlagType( const std::string& name, const std::string& abbv, FlagEndurance _endurance,
              ShotType sType, FlagQuality quality, TeamColor team, FlagEffect effect, const std::string& help,
              bool _custom = false ) :
        flagName(name),
        flagAbbv(abbv),
        flagHelp(help)
    {
        endurance = _endurance;
        flagShot = sType;
        flagQuality = quality;
        flagTeam = team;
        flagEffect = effect;
        custom = _custom;
    }

    /** returns a label of flag name and abbreviation with the flag name
     * excentuating the abbreviation if relevant.
     */
    const std::string label() const;

    /** returns information about a flag including the name, abbreviation, and
     * description.  format is "name ([+|-]abbrev): description" where +|-
     * indicates whether the flag is inherently good or bad by default.
     */
    const std::string information() const;

    /** returns the color of the flag */
    const float* getColor() const;

    /** returns the color of the flag as it should be shown on the radar */
    const float* getRadarColor() const;

    /** network serialization */
    void* pack(void* buf) const;
    void* fakePack(void* buf) const;
    void* packCustom(void* buf) const;

    /** network deserialization */
    static const void* unpack(const void* buf, FlagType::Ptr &desc);
    static const void* unpackCustom(const void* buf, FlagType::Ptr &desc);

    /** Static wrapper function that makes sure that the flag map is
     * initialized before it's used.
     */
    static TypeMap& getFlagMap();

    const std::string flagName;
    const std::string flagAbbv;
    const std::string flagHelp;
    FlagEndurance endurance;
    FlagQuality flagQuality;
    ShotType flagShot;
    TeamColor flagTeam;
    FlagEffect flagEffect = FlagEffect::Normal;
    bool custom;

    static std::vector<Set> Sets;
    static Set customFlags;
    static const int packSize;

    /** This function returns a set of all good flagtypes that are available in
    the game.
    @see FlagType
    @see FlagQuality
    */
    static FlagType::Set& getGoodFlags();

    /** This function returns a set of all bad flagtypes that are available in
    the game.
    @see FlagType
    @see FlagQuality
    */
    static FlagType::Set& getBadFlags();

    /** This function returns a pointer to the FlagType object that is associated
    with the given abbreviation. If there is no such FlagType object, NULL
    is returned. */
    static FlagType::Ptr getDescFromAbbreviation(const char* abbreviation);
};


/** This class represents an actual flag. It has functions for serialization
    and deserialization as well as static functions that returns sets of
    all good or bad flags, and maps flag abbreviations to FlagType objects. */
class FlagInstance
{
public:
    typedef std::shared_ptr<FlagInstance> Ptr;

    /** This function serializes this object into a @c void* buffer for network
        transfer. */
    void* pack(void*) const;
    /** This function serializes this object into a @c void* buffer for network
        transfer. */
    void* fakePack(void*) const;
    /** This function uses the given serialization to set the member variables
        of this object. This really hide the type of flag */
    const void* unpack(const void*);

    FlagType::Ptr type;
    FlagStatus status;
    FlagEndurance endurance;
    PlayerId owner;       // who has flag
    float position[3];        // position on ground
    float launchPosition[3];  // position flag launched from
    float landingPosition[3]; // position flag will land
    float flightTime;     // flight time so far
    float flightEnd;      // total duration of flight
    float initialVelocity;    // initial launch velocity
};

/** Flags no longer use enumerated IDs. Over the wire, flags are all
    represented by their abbreviation, null-padded to two bytes. Internally,
    flags are now represented by pointers to singleton FlagType classes.

    For more information about these flags, see Flag.cxx where these FlagType
    instances are created.
*/
namespace Flags
{
extern FlagType::Ptr Null;
extern FlagType::Ptr RedTeam;
extern FlagType::Ptr GreenTeam;
extern FlagType::Ptr BlueTeam;
extern FlagType::Ptr PurpleTeam;
extern FlagType::Ptr Velocity;
extern FlagType::Ptr QuickTurn;
extern FlagType::Ptr OscillationOverthruster;
extern FlagType::Ptr RapidFire;
extern FlagType::Ptr MachineGun;
extern FlagType::Ptr GuidedMissile;
extern FlagType::Ptr Laser;
extern FlagType::Ptr Ricochet;
extern FlagType::Ptr SuperBullet;
extern FlagType::Ptr InvisibleBullet;
extern FlagType::Ptr Stealth;
extern FlagType::Ptr Tiny;
extern FlagType::Ptr Narrow;
extern FlagType::Ptr Shield;
extern FlagType::Ptr Steamroller;
extern FlagType::Ptr ShockWave;
extern FlagType::Ptr PhantomZone;
extern FlagType::Ptr Jumping;
extern FlagType::Ptr Identify;
extern FlagType::Ptr Cloaking;
extern FlagType::Ptr Useless;
extern FlagType::Ptr Masquerade;
extern FlagType::Ptr Seer;
extern FlagType::Ptr Thief;
extern FlagType::Ptr Burrow;
extern FlagType::Ptr Wings;
extern FlagType::Ptr Agility;
extern FlagType::Ptr Colorblindness;
extern FlagType::Ptr Obesity;
extern FlagType::Ptr LeftTurnOnly;
extern FlagType::Ptr RightTurnOnly;
extern FlagType::Ptr ForwardOnly;
extern FlagType::Ptr ReverseOnly;
extern FlagType::Ptr Momentum;
extern FlagType::Ptr Blindness;
extern FlagType::Ptr Jamming;
extern FlagType::Ptr WideAngle;
extern FlagType::Ptr NoJumping;
extern FlagType::Ptr TriggerHappy;
extern FlagType::Ptr ReverseControls;
extern FlagType::Ptr Bouncy;
extern FlagType::Ptr Unknown;

/** This function initializes all the FlagType objects in the Flags
    namespace. */
void init();
void kill();

/** Clear all the custom flags (i.e. when switching servers) */
void clearCustomFlags();

FlagType::Ptr AddCustomFlag(FlagType::Ptr ptr);
}

#endif // BZF_FLAG_H

// Local Variables: ***
// mode: C++ ***
// tab-width: 4 ***
// c-basic-offset: 4 ***
// indent-tabs-mode: nil ***
// End: ***
// ex: shiftwidth=4 tabstop=4
