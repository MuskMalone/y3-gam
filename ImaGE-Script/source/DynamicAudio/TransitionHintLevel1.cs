using System;
using IGE.Utils;
using System.Numerics;

public class TransitionHintLevel1 : Entity
{
    public Entity motherVoice;
    public Entity player;

    void Update()
    {
        if (InternalCalls.OnTriggerEnter(motherVoice.mEntityID, player.mEntityID))
        {
            InternalCalls.PlaySound(motherVoice.mEntityID, "UpHereTaunt");
        }
    }
}
