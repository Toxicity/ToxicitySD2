/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "GameEventMgr.h"

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_innkeeper            25%    ScriptName not assigned. Innkeepers in general.
npc_lunaclaw_spirit     100%    Appears at two different locations, quest 6001/6002
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_rogue_trainer        80%    Scripted trainers, so they are able to offer item 17126 for class quest 6681
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
EndContentData */

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon creature at smaller range
    SPAWNTYPE_ALARMBOT                                      // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 m_uiThisCreatureEntry;
    uint32 m_uiSpawnedCreatureEntry;
    SpawnType m_SpawnType;
};

enum
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 5000
};

const float RANGE_TRIPWIRE          = 15.0f;
const float RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation m_aSpawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

struct MANGOS_DLL_DECL npc_air_force_botsAI : public ScriptedAI
{
    npc_air_force_botsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pSpawnAssoc = NULL;

        // find the correct spawnhandling
        static uint32 uiEntryCount = sizeof(m_aSpawnAssociations)/sizeof(SpawnAssociation);

        for (uint8 i=0; i<uiEntryCount; ++i)
        {
            if (m_aSpawnAssociations[i].m_uiThisCreatureEntry == pCreature->GetEntry())
            {
                m_pSpawnAssoc = &m_aSpawnAssociations[i];
                break;
            }
        }

        if (!m_pSpawnAssoc)
            error_db_log("SD2: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", pCreature->GetEntry());
        else
        {
            CreatureInfo const* spawnedTemplate = GetCreatureTemplateStore(m_pSpawnAssoc->m_uiSpawnedCreatureEntry);

            if (!spawnedTemplate)
            {
                error_db_log("SD2: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
                m_pSpawnAssoc = NULL;
                return;
            }
        }
    }

    SpawnAssociation* m_pSpawnAssoc;
    ObjectGuid m_spawnedGuid;

    void Reset() { }

    Creature* SummonGuard()
    {
        Creature* pSummoned = m_creature->SummonCreature(m_pSpawnAssoc->m_uiSpawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

        if (pSummoned)
            m_spawnedGuid = pSummoned->GetObjectGuid();
        else
        {
            error_db_log("SD2: npc_air_force_bots: wasn't able to spawn creature %u", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
            m_pSpawnAssoc = NULL;
        }

        return pSummoned;
    }

    Creature* GetSummonedGuard()
    {
        Creature* pCreature = m_creature->GetMap()->GetCreature(m_spawnedGuid);

        if (pCreature && pCreature->isAlive())
            return pCreature;

        return NULL;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pSpawnAssoc)
            return;

        if (pWho->isTargetableForAttack() && m_creature->IsHostileTo(pWho))
        {
            Player* pPlayerTarget = pWho->GetTypeId() == TYPEID_PLAYER ? (Player*)pWho : NULL;

            // airforce guards only spawn for players
            if (!pPlayerTarget)
                return;

            Creature* pLastSpawnedGuard = m_spawnedGuid ? GetSummonedGuard() : NULL;

            // prevent calling GetCreature at next MoveInLineOfSight call - speedup
            if (!pLastSpawnedGuard)
                m_spawnedGuid.Clear();

            switch(m_pSpawnAssoc->m_SpawnType)
            {
                case SPAWNTYPE_ALARMBOT:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_GUARDS_MARK))
                        return;

                    Aura* pMarkAura = pWho->GetAura(SPELL_GUARDS_MARK, EFFECT_INDEX_0);
                    if (pMarkAura)
                    {
                        // the target wasn't able to move out of our range within 25 seconds
                        if (!pLastSpawnedGuard)
                        {
                            pLastSpawnedGuard = SummonGuard();

                            if (!pLastSpawnedGuard)
                                return;
                        }

                        if (pMarkAura->GetAuraDuration() < AURA_DURATION_TIME_LEFT)
                        {
                            if (!pLastSpawnedGuard->getVictim())
                                pLastSpawnedGuard->AI()->AttackStart(pWho);
                        }
                    }
                    else
                    {
                        if (!pLastSpawnedGuard)
                            pLastSpawnedGuard = SummonGuard();

                        if (!pLastSpawnedGuard)
                            return;

                        pLastSpawnedGuard->CastSpell(pWho, SPELL_GUARDS_MARK, true);
                    }
                    break;
                }
                case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_TRIPWIRE))
                        return;

                    if (!pLastSpawnedGuard)
                        pLastSpawnedGuard = SummonGuard();

                    if (!pLastSpawnedGuard)
                        return;

                    // ROOFTOP only triggers if the player is on the ground
                    if (!pPlayerTarget->IsFlying())
                    {
                        if (!pLastSpawnedGuard->getVictim())
                            pLastSpawnedGuard->AI()->AttackStart(pWho);
                    }
                    break;
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_air_force_bots(Creature* pCreature)
{
    return new npc_air_force_botsAI(pCreature);
}

/*########
# npc_chicken_cluck
#########*/

enum
{
    EMOTE_A_HELLO           = -1000204,
    EMOTE_H_HELLO           = -1000205,
    EMOTE_CLUCK_TEXT2       = -1000206,

    QUEST_CLUCK             = 3861,
    FACTION_FRIENDLY        = 35,
    FACTION_CHICKEN         = 31
};

struct MANGOS_DLL_DECL npc_chicken_cluckAI : public ScriptedAI
{
    npc_chicken_cluckAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset()
    {
        m_uiResetFlagTimer = 120000;

        m_creature->setFaction(FACTION_CHICKEN);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (uiEmote == TEXTEMOTE_CHICKEN)
        {
            if (!urand(0, 29))
            {
                if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE)
                {
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    m_creature->setFaction(FACTION_FRIENDLY);

                    DoScriptText(EMOTE_A_HELLO, m_creature);

                    /* are there any difference in texts, after 3.x ?
                    if (pPlayer->GetTeam() == HORDE)
                        DoScriptText(EMOTE_H_HELLO, m_creature);
                    else
                        DoScriptText(EMOTE_A_HELLO, m_creature);
                    */
                }
            }
        }

        if (uiEmote == TEXTEMOTE_CHEER)
        {
            if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
            {
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->setFaction(FACTION_FRIENDLY);
                DoScriptText(EMOTE_CLUCK_TEXT2, m_creature);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        {
            if (m_uiResetFlagTimer < uiDiff)
                EnterEvadeMode();
            else
                m_uiResetFlagTimer -= uiDiff;
        }

        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_chicken_cluck(Creature* pCreature)
{
    return new npc_chicken_cluckAI(pCreature);
}

bool QuestAccept_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

bool QuestRewarded_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

/*######
## npc_dancing_flames
######*/

enum
{
    SPELL_FIERY_SEDUCTION = 47057
};

struct MANGOS_DLL_DECL npc_dancing_flamesAI : public ScriptedAI
{
    npc_dancing_flamesAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        m_creature->SetFacingToObject(pPlayer);

        if (pPlayer->HasAura(SPELL_FIERY_SEDUCTION))
            pPlayer->RemoveAurasDueToSpell(SPELL_FIERY_SEDUCTION);

        if (pPlayer->IsMounted())
        {
            pPlayer->Unmount();                             // doesnt remove mount aura
            pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        }

        switch(uiEmote)
        {
            case TEXTEMOTE_DANCE: DoCastSpellIfCan(pPlayer, SPELL_FIERY_SEDUCTION); break;// dance -> cast SPELL_FIERY_SEDUCTION
            case TEXTEMOTE_WAVE:  m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);      break;// wave -> wave
            case TEXTEMOTE_JOKE:  m_creature->HandleEmote(EMOTE_STATE_LAUGH);       break;// silly -> laugh(with sound)
            case TEXTEMOTE_BOW:   m_creature->HandleEmote(EMOTE_ONESHOT_BOW);       break;// bow -> bow
            case TEXTEMOTE_KISS:  m_creature->HandleEmote(TEXTEMOTE_CURTSEY);       break;// kiss -> curtsey
        }
    }
};

CreatureAI* GetAI_npc_dancing_flames(Creature* pCreature)
{
    return new npc_dancing_flamesAI(pCreature);
}

/*######
## Triage quest
######*/

enum
{
    SAY_DOC1                    = -1000201,
    SAY_DOC2                    = -1000202,
    SAY_DOC3                    = -1000203,

    QUEST_TRIAGE_H              = 6622,
    QUEST_TRIAGE_A              = 6624,

    DOCTOR_ALLIANCE             = 12939,
    DOCTOR_HORDE                = 12920,
    ALLIANCE_COORDS             = 7,
    HORDE_COORDS                = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                  // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                  // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                  // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                  // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                  // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                  // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                  // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                  // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                  // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                  // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                  // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                  // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f}                   // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

const uint32 AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

const uint32 HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/

struct MANGOS_DLL_DECL npc_doctorAI : public ScriptedAI
{
    npc_doctorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_playerGuid;

    uint32 SummonPatient_Timer;
    uint32 SummonPatientCount;
    uint32 PatientDiedCount;
    uint32 PatientSavedCount;

    bool Event;

    GUIDList Patients;
    std::vector<Location*> Coordinates;

    void Reset()
    {
        m_playerGuid.Clear();

        SummonPatient_Timer = 10000;
        SummonPatientCount = 0;
        PatientDiedCount = 0;
        PatientSavedCount = 0;

        Patients.clear();
        Coordinates.clear();

        Event = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void BeginEvent(Player* pPlayer);
    void PatientDied(Location* Point);
    void PatientSaved(Creature* soldier, Player* pPlayer, Location* Point);
    void UpdateAI(const uint32 diff);
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

struct MANGOS_DLL_DECL npc_injured_patientAI : public ScriptedAI
{
    npc_injured_patientAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_doctorGuid;
    Location* Coord;

    void Reset()
    {
        m_doctorGuid.Clear();
        Coord = NULL;

        //no select
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //no regen health
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        //to make them lay with face down
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        uint32 mobId = m_creature->GetEntry();

        switch (mobId)
        {                                                   //lower max health
            case 12923:
            case 12938:                                     //Injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.75));
                break;
            case 12924:
            case 12936:                                     //Badly injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.50));
                break;
            case 12925:
            case 12937:                                     //Critically injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.25));
                break;
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == 20804)
        {
            if ((((Player*)caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (((Player*)caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
            {
                if (m_doctorGuid)
                {
                    if (Creature* pDoctor = m_creature->GetMap()->GetCreature(m_doctorGuid))
                    {
                        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                            pDocAI->PatientSaved(m_creature, (Player*)caster, Coord);
                    }
                }
            }
            //make not selectable
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //regen health
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            //stand up
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_DOC1,m_creature); break;
                case 1: DoScriptText(SAY_DOC2,m_creature); break;
                case 2: DoScriptText(SAY_DOC3,m_creature); break;
            }

            m_creature->SetWalk(false);

            uint32 mobId = m_creature->GetEntry();

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    m_creature->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    m_creature->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //lower HP on every world tick makes it a useful counter, not officlone though
        uint32 uiHPLose = uint32(0.05f * diff);
        if (m_creature->isAlive() && m_creature->GetHealth() > 1 + uiHPLose)
        {
            m_creature->SetHealth(m_creature->GetHealth() - uiHPLose);
        }

        if (m_creature->isAlive() && m_creature->GetHealth() <= 1 + uiHPLose)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDeathState(JUST_DIED);
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            if (m_doctorGuid)
            {
                if (Creature* pDoctor = m_creature->GetMap()->GetCreature(m_doctorGuid))
                {
                    if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                        pDocAI->PatientDied(Coord);
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_injured_patient(Creature* pCreature)
{
    return new npc_injured_patientAI(pCreature);
}

/*
npc_doctor (continue)
*/

void npc_doctorAI::BeginEvent(Player* pPlayer)
{
    m_playerGuid = pPlayer->GetObjectGuid();

    SummonPatient_Timer = 10000;
    SummonPatientCount = 0;
    PatientDiedCount = 0;
    PatientSavedCount = 0;

    switch(m_creature->GetEntry())
    {
        case DOCTOR_ALLIANCE:
            for(uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                Coordinates.push_back(&AllianceCoords[i]);
            break;
        case DOCTOR_HORDE:
            for(uint8 i = 0; i < HORDE_COORDS; ++i)
                Coordinates.push_back(&HordeCoords[i]);
            break;
    }

    Event = true;
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void npc_doctorAI::PatientDied(Location* Point)
{
    Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

    if (pPlayer && ((pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
    {
        ++PatientDiedCount;

        if (PatientDiedCount > 5 && Event)
        {
            if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_A);
            else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_H);

            Reset();
            return;
        }

        Coordinates.push_back(Point);
    }
    else
        // If no player or player abandon quest in progress
        Reset();
}

void npc_doctorAI::PatientSaved(Creature* soldier, Player* pPlayer, Location* Point)
{
    if (pPlayer && m_playerGuid == pPlayer->GetObjectGuid())
    {
        if ((pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE))
        {
            ++PatientSavedCount;

            if (PatientSavedCount == 15)
            {
                for(GUIDList::const_iterator itr = Patients.begin(); itr != Patients.end(); ++itr)
                {
                    if (Creature* Patient = m_creature->GetMap()->GetCreature(*itr))
                        Patient->SetDeathState(JUST_DIED);
                }

                if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_A, m_creature);
                else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_H, m_creature);

                Reset();
                return;
            }

            Coordinates.push_back(Point);
        }
    }
}

void npc_doctorAI::UpdateAI(const uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatient_Timer < diff)
        {
            Creature* Patient = NULL;
            Location* Point = NULL;

            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin()+rand()%Coordinates.size();
            uint32 patientEntry = 0;

            switch(m_creature->GetEntry())
            {
                case DOCTOR_ALLIANCE: patientEntry = AllianceSoldierId[urand(0, 2)]; break;
                case DOCTOR_HORDE:    patientEntry = HordeSoldierId[urand(0, 2)]; break;
                default:
                    error_log("SD2: Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            Point = *itr;

            Patient = m_creature->SummonCreature(patientEntry, Point->x, Point->y, Point->z, Point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

            if (Patient)
            {
                //2.4.3, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP);

                Patients.push_back(Patient->GetObjectGuid());

                npc_injured_patientAI* pPatientAI = dynamic_cast<npc_injured_patientAI*>(Patient->AI());

                if (pPatientAI)
                {
                    pPatientAI->m_doctorGuid = m_creature->GetObjectGuid();

                    if (Point)
                        pPatientAI->Coord = Point;
                }

                Coordinates.erase(itr);
            }
            SummonPatient_Timer = 10000;
            ++SummonPatientCount;
        }else SummonPatient_Timer -= diff;
    }
}

bool QuestAccept_npc_doctor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if ((pQuest->GetQuestId() == QUEST_TRIAGE_A) || (pQuest->GetQuestId() == QUEST_TRIAGE_H))
    {
        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pCreature->AI()))
            pDocAI->BeginEvent(pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_doctor(Creature* pCreature)
{
    return new npc_doctorAI(pCreature);
}

/*######
## npc_garments_of_quests
######*/

enum
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263,
};

struct MANGOS_DLL_DECL npc_garments_of_questsAI : public npc_escortAI
{
    npc_garments_of_questsAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    bool m_bIsHealed;
    bool m_bCanRun;

    uint32 m_uiRunAwayTimer;

    void Reset()
    {
        m_playerGuid.Clear();

        m_bIsHealed = false;
        m_bCanRun = false;

        m_uiRunAwayTimer = 5000;

        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        // expect database to have RegenHealth=0
        m_creature->SetHealth(int(m_creature->GetMaxHealth()*0.7));
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LESSER_HEAL_R2 || pSpell->Id == SPELL_FORTITUDE_R1)
        {
            // not while in combat
            if (m_creature->isInCombat())
                return;

            // nothing to be done now
            if (m_bIsHealed && m_bCanRun)
                return;

            if (pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                switch(m_creature->GetEntry())
                {
                    case ENTRY_SHAYA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                }

                // give quest credit, not expect any special quest objectives
                if (m_bCanRun)
                    ((Player*)pCaster)->TalkedToCreature(m_creature->GetEntry(), m_creature->GetObjectGuid());
            }
        }
    }

    void WaypointReached(uint32 uiPointId) {}

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (m_bCanRun && !m_creature->isInCombat())
        {
            if (m_uiRunAwayTimer <= uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    switch(m_creature->GetEntry())
                    {
                        case ENTRY_SHAYA:   DoScriptText(SAY_SHAYA_GOODBYE, m_creature, pPlayer);   break;
                        case ENTRY_ROBERTS: DoScriptText(SAY_ROBERTS_GOODBYE, m_creature, pPlayer); break;
                        case ENTRY_DOLF:    DoScriptText(SAY_DOLF_GOODBYE, m_creature, pPlayer);    break;
                        case ENTRY_KORJA:   DoScriptText(SAY_KORJA_GOODBYE, m_creature, pPlayer);   break;
                        case ENTRY_DG_KEL:  DoScriptText(SAY_DG_KEL_GOODBYE, m_creature, pPlayer);  break;
                    }

                    Start(true);
                }
                else
                    EnterEvadeMode();                       // something went wrong

                m_uiRunAwayTimer = 30000;
            }
            else
                m_uiRunAwayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_garments_of_quests(Creature* pCreature)
{
    return new npc_garments_of_questsAI(pCreature);
}

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5

struct MANGOS_DLL_DECL npc_guardianAI : public ScriptedAI
{
    npc_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->isAttackReady())
        {
            m_creature->CastSpell(m_creature->getVictim(),SPELL_DEATHTOUCH, true);
            m_creature->resetAttackTimer();
        }
    }
};

CreatureAI* GetAI_npc_guardian(Creature* pCreature)
{
    return new npc_guardianAI(pCreature);
}

/*########
# npc_innkeeper
#########*/

// Script applied to all innkeepers by npcflag.
// Are there any known innkeepers that does not hape the options in the below?
// (remember gossipHello is not called unless npcflag|1 is present)

enum
{
    TEXT_ID_WHAT_TO_DO              = 1853,

    SPELL_TRICK_OR_TREAT            = 24751,                 // create item or random buff
    SPELL_TRICK_OR_TREATED          = 24755,                 // buff player get when tricked or treated
};

#define GOSSIP_ITEM_TRICK_OR_TREAT  "Trick or Treat!"
#define GOSSIP_ITEM_WHAT_TO_DO      "What can I do at an Inn?"

bool GossipHello_npc_innkeeper(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PrepareGossipMenu(pCreature, pPlayer->GetDefaultGossipMenuForSource(pCreature));

    if (IsHolidayActive(HOLIDAY_HALLOWS_END) && !pPlayer->HasAura(SPELL_TRICK_OR_TREATED, EFFECT_INDEX_0))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRICK_OR_TREAT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    // Should only apply to innkeeper close to start areas.
    if (AreaTableEntry const* pAreaEntry = GetAreaEntryByAreaID(pCreature->GetAreaId()))
    {
        if (pAreaEntry->flags & AREA_FLAG_LOWLEVEL)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_TO_DO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    }

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_innkeeper(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_WHAT_TO_DO, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, SPELL_TRICK_OR_TREAT, true);
            break;
        case GOSSIP_OPTION_VENDOR:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_OPTION_INNKEEPER:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SetBindPoint(pCreature->GetObjectGuid());
            break;
    }

    return true;
}

/*######
## npc_lunaclaw_spirit
######*/

enum
{
    QUEST_BODY_HEART_A      = 6001,
    QUEST_BODY_HEART_H      = 6002,

    TEXT_ID_DEFAULT         = 4714,
    TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

bool GossipHello_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetObjectGuid());
        pPlayer->AreaExploredOrEventHappens((pPlayer->GetTeam() == ALLIANCE) ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
    }
    return true;
}

/*######
## npc_mount_vendor
######*/

bool GossipHello_npc_mount_vendor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    bool canBuy;
    canBuy = false;
    uint32 vendor = pCreature->GetEntry();
    uint8 race = pPlayer->getRace();

    switch (vendor)
    {
        case 384:                                           //Katie Hunter
        case 1460:                                          //Unger Statforth
        case 2357:                                          //Merideth Carlson
        case 4885:                                          //Gregor MacVince
            if (pPlayer->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                pPlayer->SEND_GOSSIP_MENU(5855, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 1261:                                          //Veron Amberstill
            if (pPlayer->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                pPlayer->SEND_GOSSIP_MENU(5856, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 3362:                                          //Ogunaro Wolfrunner
            if (pPlayer->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                pPlayer->SEND_GOSSIP_MENU(5841, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 3685:                                          //Harb Clawhoof
            if (pPlayer->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                pPlayer->SEND_GOSSIP_MENU(5843, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 4730:                                          //Lelanai
            if (pPlayer->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                pPlayer->SEND_GOSSIP_MENU(5844, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 4731:                                          //Zachariah Post
            if (pPlayer->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD)
                pPlayer->SEND_GOSSIP_MENU(5840, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 7952:                                          //Zjolnir
            if (pPlayer->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                pPlayer->SEND_GOSSIP_MENU(5842, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 7955:                                          //Milli Featherwhistle
            if (pPlayer->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                pPlayer->SEND_GOSSIP_MENU(5857, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 16264:                                         //Winaestra
            if (pPlayer->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                pPlayer->SEND_GOSSIP_MENU(10305, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 17584:                                         //Torallius the Pack Handler
            if (pPlayer->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                pPlayer->SEND_GOSSIP_MENU(10239, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
    }

    if (canBuy)
    {
        if (pCreature->isVendor())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_mount_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

/*######
## npc_rogue_trainer
######*/

bool GossipHello_npc_rogue_trainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isTrainer())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

    if (pCreature->CanTrainAndResetTalentsOf(pPlayer))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "I wish to unlearn my talents", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

    if (pPlayer->getClass() == CLASS_ROGUE && pPlayer->getLevel() >= 24 && !pPlayer->HasItemCount(17126,1) && !pPlayer->GetQuestRewardStatus(6681))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<Take the letter>", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(5996, pCreature->GetObjectGuid());
    } else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_rogue_trainer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer,21100,false);
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_OPTION_UNLEARNTALENTS:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SendTalentWipeConfirm(pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

bool GossipHello_npc_sayge(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->HasSpellCooldown(SPELL_INT) ||
        pPlayer->HasSpellCooldown(SPELL_ARM) ||
        pPlayer->HasSpellCooldown(SPELL_DMG) ||
        pPlayer->HasSpellCooldown(SPELL_RES) ||
        pPlayer->HasSpellCooldown(SPELL_STR) ||
        pPlayer->HasSpellCooldown(SPELL_AGI) ||
        pPlayer->HasSpellCooldown(SPELL_STM) ||
        pPlayer->HasSpellCooldown(SPELL_SPI))
        pPlayer->SEND_GOSSIP_MENU(7393, pCreature->GetObjectGuid());
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(7339, pCreature->GetObjectGuid());
    }

    return true;
}

void SendAction_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Slay the Man",                      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn him over to liege",            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confiscate the corn",               GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let him go and have the corn",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(7340, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painfully",     GOSSIP_SENDER_MAIN+1, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painlessly",    GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let your friend go",                GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7341, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confront the diplomat",             GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show not so quiet defiance",        GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remain quiet",                      GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7361, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Speak against your brother openly", GOSSIP_SENDER_MAIN+6, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Help your brother in",              GOSSIP_SENDER_MAIN+7, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Keep your brother out without letting him know", GOSSIP_SENDER_MAIN+8, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7362, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, keep gold",            GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, share the gold",       GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let the knight take credit",        GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7363, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks",                            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(7364, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pCreature->CastSpell(pPlayer, SPELL_FORTUNE, false);
            pPlayer->SEND_GOSSIP_MENU(7365, pCreature->GetObjectGuid());
            break;
    }
}

bool GossipSelect_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiSender)
    {
        case GOSSIP_SENDER_MAIN:
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+1:
            pCreature->CastSpell(pPlayer, SPELL_DMG, false);
            pPlayer->AddSpellCooldown(SPELL_DMG,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+2:
            pCreature->CastSpell(pPlayer, SPELL_RES, false);
            pPlayer->AddSpellCooldown(SPELL_RES,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+3:
            pCreature->CastSpell(pPlayer, SPELL_ARM, false);
            pPlayer->AddSpellCooldown(SPELL_ARM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+4:
            pCreature->CastSpell(pPlayer, SPELL_SPI, false);
            pPlayer->AddSpellCooldown(SPELL_SPI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+5:
            pCreature->CastSpell(pPlayer, SPELL_INT, false);
            pPlayer->AddSpellCooldown(SPELL_INT,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+6:
            pCreature->CastSpell(pPlayer, SPELL_STM, false);
            pPlayer->AddSpellCooldown(SPELL_STM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+7:
            pCreature->CastSpell(pPlayer, SPELL_STR, false);
            pPlayer->AddSpellCooldown(SPELL_STR,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+8:
            pCreature->CastSpell(pPlayer, SPELL_AGI, false);
            pPlayer->AddSpellCooldown(SPELL_AGI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}

void AddSC_npcs_special()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_air_force_bots";
    pNewScript->GetAI = &GetAI_npc_air_force_bots;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_chicken_cluck";
    pNewScript->GetAI = &GetAI_npc_chicken_cluck;
    pNewScript->pQuestAcceptNPC =   &QuestAccept_npc_chicken_cluck;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_chicken_cluck;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dancing_flames";
    pNewScript->GetAI = &GetAI_npc_dancing_flames;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_patient";
    pNewScript->GetAI = &GetAI_npc_injured_patient;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doctor";
    pNewScript->GetAI = &GetAI_npc_doctor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_doctor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_garments_of_quests";
    pNewScript->GetAI = &GetAI_npc_garments_of_quests;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_guardian";
    pNewScript->GetAI = &GetAI_npc_guardian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_innkeeper";
    pNewScript->pGossipHello = &GossipHello_npc_innkeeper;
    pNewScript->pGossipSelect = &GossipSelect_npc_innkeeper;
    pNewScript->RegisterSelf(false);                         // script and error report disabled, but script can be used for custom needs, adding ScriptName

    pNewScript = new Script;
    pNewScript->Name = "npc_lunaclaw_spirit";
    pNewScript->pGossipHello =  &GossipHello_npc_lunaclaw_spirit;
    pNewScript->pGossipSelect = &GossipSelect_npc_lunaclaw_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mount_vendor";
    pNewScript->pGossipHello =  &GossipHello_npc_mount_vendor;
    pNewScript->pGossipSelect = &GossipSelect_npc_mount_vendor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rogue_trainer";
    pNewScript->pGossipHello =  &GossipHello_npc_rogue_trainer;
    pNewScript->pGossipSelect = &GossipSelect_npc_rogue_trainer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sayge";
    pNewScript->pGossipHello = &GossipHello_npc_sayge;
    pNewScript->pGossipSelect = &GossipSelect_npc_sayge;
    pNewScript->RegisterSelf();
}
