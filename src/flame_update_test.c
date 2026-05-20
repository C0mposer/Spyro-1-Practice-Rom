#include <types.h>

typedef struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

typedef struct Mat33 {
    u32 packed[5];
} Mat33;

typedef void (*OverlayCreateParticleFn)(s32 kind, s32 subtype, Vec3* position, Vec3* velocity);

extern u32 rand(void);
extern s32 SinScaled(s32 angle);
extern void ArrayCopy(void* dst, void* src, s32 bytes);
extern void Vec3ApplyRotationMatrix(Mat33* matrix, Vec3* src, Vec3* dst);
extern void Vec3ApplyLastLoadedRotationMatrix(Vec3* src, Vec3* dst);
extern void Vec3Add(Vec3* dst, Vec3* a, Vec3* b);
extern void Vec3Subtract(Vec3* dst, Vec3* a, Vec3* b);
extern void Vec3Copy(Vec3* dst, Vec3* src);
extern void Vec3Zero(Vec3* dst);
extern void Vec3ShortToInt(Vec3* dst, void* packedShortVec);
extern s32 MobyCollisionRelated(Vec3* position, s32 radius, s32 arg2, u32 flags, s32 arg4, s32 arg5);
extern s32 FindCollsionPolygonAltro(Vec3* from, Vec3* to);
extern void PlaySoundEffect(u32 sound, Vec3* position, s32 arg2, s32 arg3);

#define PTR(type, address) ((type *)(address))
#define REF(type, address) (*(type *)(address))

#define frameTimeDelta REF(s32, 0x800756cc)
#define levelSoundEffects REF(u8 *, 0x800761d4)
#define spyroModelSet REF(u8 *, 0x80076378)
#define CreateOverlayParticle REF(OverlayCreateParticleFn, 0x800758e4)

#define spyroPosition REF(Vec3, 0x80078a58)
#define spyroVisualYaw REF(u8, 0x80078a64)
#define spyroVisualPitch REF(u8, 0x80078a65)
#define spyroVisualRoll REF(u8, 0x80078a66)
#define spyroAnimationState REF(u32, 0x80078a70)
#define spyroCurrentKeyframe REF(u8, 0x80078a76)
#define spyroPreviousKeyframe REF(u8, 0x80078a78)
#define spyroPreviousAnimSpeed REF(u8, 0x80078a7d)
#define spyroColorFilterRed REF(u8, 0x80078a80)
#define spyroColorFilterGreen REF(u8, 0x80078a81)
#define spyroColorFilterBlue REF(u8, 0x80078a82)
#define spyroColorFilterOpacity REF(u8, 0x80078a83)
#define spyroFramesInAir REF(s32, 0x80078af4)
#define spyroTargetHeadPitchWord REF(s32, 0x80078c00)

#define isSpyroFlaming REF(u8, 0x80078760)
#define flameSegmentSpawnIndex REF(u8, 0x80078761)
#define useAnimatedMouthAnchor REF(u8, 0x80078762)
#define flameSlotShuffleRow REF(u8, 0x80078763)
#define superFlameActive REF(s32, 0x80078764)
#define superFlameTimeRemaining REF(s32, 0x80078768)

#define flamePathMatrix REF(Mat33, 0x8007876c)
#define spyroBodyMatrix REF(Mat33, 0x80078a8c)
#define spyroFlamePathMatrixSource ((void *)0x80078c20)
#define activeFlameOrigin REF(Vec3, 0x800786c8)
#define flameBaseVisualAngles PTR(u8, 0x800786d4)
#define flameSegmentAges PTR(u8, 0x800786e8)
#define flameImpactAges PTR(u8, 0x800786f0)
#define flameSegmentStates PTR(u8, 0x800786f8)
#define flameOriginOffsetFromSpyro REF(Vec3, 0x80078794)
#define previousFlameSegmentPositions PTR(Vec3, 0x800787a0)

#define flameSlotShuffleTable PTR(u8, 0x8006e1c8)
#define flameMouthAnchorOffsets PTR(Vec3, 0x8006da6c)
#define fallbackFlameMouthAnchorOffset REF(Vec3, 0x8006da9c)
#define shortFlamePathSamples ((void *)0x8006daa8)
#define longFlamePathSamples ((void *)0x8006dda8)
#define flameEndVelocityTable PTR(Vec3, 0x8006e1d8)

static s32 DecodeSigned11FromPackedVertex(u32 packed, s32 shiftLeft)
{
    return (s32)(packed << shiftLeft) >> 21;
}

J_HOOK(0x80048d10)
void FlameUpdate(s32 deltaFrames)
{
    Vec3 flamePointWorldPosition;
    Vec3 flameAttachmentVector;
    Vec3 traceStartOrParticleVelocity;
    s32 inactiveFlameCount;
    s32 playedImpactSound;
    s32 frameStep;
    s32 segmentIndex;
    s32 pathSampleCount;
    s32 segmentExpireAge;
    s32 pathSamplesBase;
    u32 segmentAge;
    s32 hitDetected;
    u32 flameCollisionMask;
    s32 terminalFlameSubtype;
    s32 packedMouthVertex;
    u32 packedFrameDescriptor;
    u32 spyroAnimationId;

    inactiveFlameCount = 0;
    playedImpactSound = 0;

    // Keep Spyro's super-flame aura alive with drifting fire particles and a pulsing color filter.
    if (superFlameTimeRemaining != 0) {
        // Scatter a small particle cloud around Spyro, biased backward along the local flame direction.
        flamePointWorldPosition.x = (rand() & 0x1ff) - 0x160;
        flamePointWorldPosition.y = (rand() & 0x7f) - 0x40;
        flamePointWorldPosition.z = (flamePointWorldPosition.x >> 1) + (rand() & 0x3f) - 0x60;

        // Move the random offset into Spyro's world space before spawning the ambient flame particle.
        Vec3ApplyRotationMatrix(&spyroBodyMatrix, &flamePointWorldPosition, &flamePointWorldPosition);
        Vec3Add(&flamePointWorldPosition, &flamePointWorldPosition, &spyroPosition);
        CreateOverlayParticle(1, 0x1e, &flamePointWorldPosition, 0);

        // Tint Spyro orange while the super-flame powerup is active.
        spyroColorFilterRed = 0x90;
        spyroColorFilterGreen = 0x20;
        spyroColorFilterBlue = 0x10;

        // Blink near the end of the timer, otherwise pulse opacity with a sine wave.
        if (superFlameTimeRemaining < 0x78 && superFlameTimeRemaining % 0x18 > 0x0d) {
            spyroColorFilterOpacity = 0;
        }
        else {
            s32 opacityPulse = SinScaled(superFlameTimeRemaining << 7);
            if (opacityPulse < 0) {
                opacityPulse += 0xff;
            }
            spyroColorFilterOpacity = (u8)((opacityPulse >> 8) + 0xd8);
        }

        superFlameTimeRemaining -= frameTimeDelta;
        if (superFlameTimeRemaining <= 0) {
            superFlameTimeRemaining = 0;
            spyroColorFilterOpacity = 0;
        }
    }

    // Nothing else updates once every flame segment has expired.
    if (isSpyroFlaming == 0) {
        return;
    }

    // Tilt Spyro's head toward the flame only while grounded and pitching upward.
    if (spyroFramesInAir == 0 && spyroVisualPitch > 0x80) {
        spyroTargetHeadPitchWord = (0x100 - spyroVisualPitch) << 3;
    }
    else {
        spyroTargetHeadPitchWord = 0;
    }

    // Queue new flame trail segments for this frame. The shuffle table spreads consecutive
    // segments across slots so multiple visible flame points can age independently.
    for (frameStep = 0; frameStep < deltaFrames; frameStep++) {
        if (flameSegmentSpawnIndex < 8) {
            u8 shuffledSlot = flameSlotShuffleTable[flameSlotShuffleRow * 8 + flameSegmentSpawnIndex];
            flameSegmentAges[shuffledSlot] = (u8)(deltaFrames - frameStep);
            flameImpactAges[shuffledSlot] = 0x20;
            flameSegmentStates[shuffledSlot] = 0;
        }
        flameSegmentSpawnIndex++;
    }

    // Hold the flame animation briefly while the early flame trail is still being emitted.
    if (flameSegmentSpawnIndex < 0x1e &&
        (spyroAnimationState & 0xffff0000) == 0x07070000 &&
        spyroPreviousKeyframe == 4) {
        spyroPreviousAnimSpeed = 0;
    }

    // Establish the world-space origin that all flame path points will be added to.
    if (useAnimatedMouthAnchor == 0) {
        // Use the existing flame origin when the mouth attachment does not need animation data.
        Vec3Add(&activeFlameOrigin, &spyroPosition, &flameOriginOffsetFromSpyro);
    }
    else {
        // Capture Spyro's current facing angles and the matrix that orients the flame path.
        flameBaseVisualAngles[0] = spyroVisualYaw;
        flameBaseVisualAngles[1] = spyroVisualPitch;
        flameBaseVisualAngles[2] = spyroVisualRoll;
        ArrayCopy(&flamePathMatrix.packed[0], spyroFlamePathMatrixSource, 0x14);

        // Read the current keyframe's packed vertex pointer so the flame can start at Spyro's mouth.
        spyroAnimationId = spyroAnimationState & 0xff;
        packedFrameDescriptor = *(u32*)(*(u32*)(spyroModelSet + 0x38 + spyroAnimationId * 4) +
            0x24 + spyroCurrentKeyframe * 4); // animation table entry for this frame
        packedMouthVertex = *(s32*)(((packedFrameDescriptor & 0x1fffff) << 1) + 0x10); // 21-bit vertex data address

        // Unpack the mouth attachment vertex from signed 11-bit components.
        flameAttachmentVector.x = packedMouthVertex >> 21;
        flameAttachmentVector.y = DecodeSigned11FromPackedVertex((u32)packedMouthVertex, 11);
        flameAttachmentVector.z = DecodeSigned11FromPackedVertex((u32)packedMouthVertex, 22);

        // Place the mouth vertex in world space so the flame begins at Spyro's animated mouth.
        Vec3ApplyRotationMatrix(&spyroBodyMatrix, &flameAttachmentVector, &flameAttachmentVector);
        Vec3Add(&activeFlameOrigin, &flameAttachmentVector, &spyroPosition);

        // Pick the small forward offset that keeps new flame points attached to the current trail.
        if (flameSegmentSpawnIndex < 8) {
            Vec3Copy(&flameAttachmentVector, &flameMouthAnchorOffsets[flameSegmentSpawnIndex >> 1]);
        }
        else {
            Vec3Copy(&flameAttachmentVector, &fallbackFlameMouthAnchorOffset);
        }

        // Combine the animated mouth position and flame-path offset, then remember the
        // difference from Spyro's position for the next frame's anchored flame origin.
        Vec3ApplyRotationMatrix(&flamePathMatrix, &flameAttachmentVector, &flameAttachmentVector);
        Vec3Add(&activeFlameOrigin, &activeFlameOrigin, &flameAttachmentVector);
        Vec3Subtract(&flameOriginOffsetFromSpyro, &activeFlameOrigin, &spyroPosition);
    }

    // Update every active flame segment, moving each one along its path and spawning hit effects.
    for (segmentIndex = 0; segmentIndex < 8; segmentIndex++) {
        if (flameSegmentAges[segmentIndex] == 0) {
            inactiveFlameCount++;
            continue;
        }

        // Early slots use the shorter flame path; later slots use the longer reaching path.
        if (segmentIndex < 4) {
            pathSampleCount = 0x18;
            segmentExpireAge = 0x20;
            pathSamplesBase = (s32)shortFlamePathSamples + segmentIndex * pathSampleCount * 8;
        }
        else {
            pathSampleCount = 0x20;
            segmentExpireAge = 0x28;
            pathSamplesBase = (s32)longFlamePathSamples + (segmentIndex - 4) * pathSampleCount * 8;
        }

        // Once a segment is active, advance it farther down the flame path each frame.
        if (flameSegmentStates[segmentIndex] != 0) {
            flameSegmentAges[segmentIndex] = (u8)(flameSegmentAges[segmentIndex] + deltaFrames);
        }

        // Drop flame segments that have moved past their visible lifetime.
        if (flameSegmentAges[segmentIndex] >= segmentExpireAge) {
            flameSegmentAges[segmentIndex] = 0;
            continue;
        }

        // Impacted segments stay frozen at their hit point and no longer trace forward.
        if (flameSegmentStates[segmentIndex] == 2) {
            continue;
        }

        segmentAge = flameSegmentAges[segmentIndex];
        if ((s32)segmentAge < pathSampleCount - 1) {
            // Convert the next path sample into world space to trace the flame forward.
            Vec3ShortToInt(&flamePointWorldPosition, (void*)(pathSamplesBase + (segmentAge + 1) * 8));
            Vec3ApplyRotationMatrix(&flamePathMatrix, &flamePointWorldPosition, &flamePointWorldPosition);
            Vec3Add(&flamePointWorldPosition, &flamePointWorldPosition, &activeFlameOrigin);

            // Collision traces from Spyro or the previous flame point to the new flame point.
            if (flameSegmentStates[segmentIndex] == 0) {
                Vec3Copy(&traceStartOrParticleVelocity, &spyroPosition);
                flameSegmentStates[segmentIndex] = 1;
            }
            else {
                Vec3Copy(&traceStartOrParticleVelocity, &previousFlameSegmentPositions[segmentIndex]);
            }

            Vec3Copy(&previousFlameSegmentPositions[segmentIndex], &flamePointWorldPosition);

            // Check moby and world collision before allowing the flame segment to continue.
            if (superFlameActive != 0) {
                flameCollisionMask = 0x90000;
            }
            else {
                flameCollisionMask = 0x10000;
            }
            hitDetected = MobyCollisionRelated(&flamePointWorldPosition, 0xa0, 0, flameCollisionMask, 0, 0);
            if (hitDetected == 0) {
                hitDetected = FindCollsionPolygonAltro(&traceStartOrParticleVelocity, &flamePointWorldPosition);
            }

            if (hitDetected != 0) {
                // Freeze this segment at the hit point and emit the visible impact burst.
                flameSegmentStates[segmentIndex] = 2;
                flameImpactAges[segmentIndex] = flameSegmentAges[segmentIndex];

                // Kick a small ember sideways from the impact using the flame orientation.
                traceStartOrParticleVelocity.x = -0x10;
                traceStartOrParticleVelocity.y = (rand() & 0x1f) - 0x10;
                traceStartOrParticleVelocity.z = 0;
                Vec3ApplyRotationMatrix(&flamePathMatrix, &traceStartOrParticleVelocity, &traceStartOrParticleVelocity);
                traceStartOrParticleVelocity.z = 0;
                CreateOverlayParticle(1, 1, &flamePointWorldPosition, &traceStartOrParticleVelocity);

                if (superFlameActive == 0) {
                    CreateOverlayParticle(4, 0x4e, &flamePointWorldPosition, 0);
                }

                // Spawn the forward impact puff in the last loaded rotation space.
                traceStartOrParticleVelocity.x = 0x30;
                traceStartOrParticleVelocity.y = 0;
                traceStartOrParticleVelocity.z = 0;
                Vec3ApplyLastLoadedRotationMatrix(&traceStartOrParticleVelocity, &traceStartOrParticleVelocity);
                CreateOverlayParticle(superFlameActive * 5 + 5, 0x4f, &flamePointWorldPosition, &traceStartOrParticleVelocity);

                // Play the impact sound once even if several segments collide this frame.
                if (playedImpactSound == 0) {
                    playedImpactSound = 1;
                    PlaySoundEffect(levelSoundEffects[0x28], &spyroPosition, 4, 0);
                }
            }
        }
        else if ((s32)segmentAge > pathSampleCount && (s32)segmentAge <= pathSampleCount + deltaFrames) {
            // The segment reached the end of its path without hitting anything, so spawn the
            // terminal flame puff at the final path sample.
            Vec3ShortToInt(&flamePointWorldPosition, (void*)(pathSamplesBase + (pathSampleCount - 1) * 8));
            Vec3ApplyRotationMatrix(&flamePathMatrix, &flamePointWorldPosition, &flamePointWorldPosition);
            Vec3Add(&flamePointWorldPosition, &flamePointWorldPosition, &activeFlameOrigin);
            Vec3Zero(&traceStartOrParticleVelocity);
            if (superFlameActive != 0) {
                terminalFlameSubtype = 1;
            }
            else {
                terminalFlameSubtype = 0;
            }
            CreateOverlayParticle(1, terminalFlameSubtype, &flamePointWorldPosition, &traceStartOrParticleVelocity);

            if (superFlameActive == 0) {
                CreateOverlayParticle(4, 0x4e, &flamePointWorldPosition, 0);
            }

            Vec3ApplyLastLoadedRotationMatrix(&flameEndVelocityTable[segmentIndex], &traceStartOrParticleVelocity);
            CreateOverlayParticle(superFlameActive * 5 + 5, 0x4f, &flamePointWorldPosition, &traceStartOrParticleVelocity);
        }
    }

    // Shut the flame system off after every segment has expired.
    if (inactiveFlameCount == 8) {
        isSpyroFlaming = 0;
    }
}
