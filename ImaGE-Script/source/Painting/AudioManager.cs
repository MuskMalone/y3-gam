//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;

//using IGE.Utils;
//public class AudioManager : Entity
//{
//  //[Header("------------ Audio Source ------------")]
//  //[SerializeField] AudioSource musicSource;
//  //[SerializeField] AudioSource SFXSource;
//  //[SerializeField] AudioSource FootstepsSource;
//  //[SerializeField] AudioSource ScreamSource;
//  //[SerializeField] AudioSource BGMSource;
//    public AudioSource musicSource;
//    public AudioSource SFXSource;
//    public AudioSource FootstepsSource;
//    public AudioSource ScreamSource;
//    public AudioSource BGMSource;

//  //[Header("------------ Audio Clips ------------")]
//    public AudioClip background;
//    public AudioClip BGM;
//    public AudioClip itemPickup;
//    public AudioClip paintingMatch;
  
//    //[Header("------------ Footsteps Clips ------------")]
//    public AudioClip[] grassFootsteps;  
//    public AudioClip[] pavementFootsteps;  
//    public AudioClip[] metalpipesFootsteps;

//    //[Header("------------ Screaming Clips ------------")]
//    public AudioClip[] soulScreaming;

//    private void Start()
//    {
//        // Play the main background music
//        musicSource.clip = background;
//        musicSource.loop = true;   // Ensure looping for continuous playback
//        musicSource.Play();

//        // Play the secondary background music (or ambient track)
//        BGMSource.clip = BGM;
//        BGMSource.loop = true;     // Ensure looping for continuous playback
//        BGMSource.Play();
//    }

//    public void PlaySFX(AudioClip clip)
//    {
//        SFXSource.PlayOneShot(clip);
//    }

//    public void PlayFootsteps(AudioClip clip)
//    {
//        if (FootstepsSource.clip != clip || !FootstepsSource.isPlaying)
//        {
//            FootstepsSource.clip = clip;
//            FootstepsSource.loop = false;  
//            FootstepsSource.Play();      
//        }
//    }

//    public void PlayScreams(AudioClip clip)
//    {
//        if (ScreamSource.clip != clip || !ScreamSource.isPlaying)
//        {
//            ScreamSource.clip = clip;
//            ScreamSource.loop = false;
//            ScreamSource.PlayOneShot(clip);
//        }
//    }

//    public void PlayRandomScreams()
//    {
//        if (soulScreaming.Length > 0)
//        {
//            int randomIndex = Mathf.RandRange(0, soulScreaming.Length);
//            PlayScreams(soulScreaming[randomIndex]);
//        }
//    }

//    public void PlayRandomGrassFootstep()
//    {
//        if (grassFootsteps.Length > 0)
//        {
//            int randomIndex = Mathf.RandRange(0, grassFootsteps.Length);
//            PlayFootsteps(grassFootsteps[randomIndex]);  
//        }
//    }

//    public void PlayRandomPavementFootstep()
//    {
//        if (pavementFootsteps.Length > 0)
//        {
//            int randomIndex = Mathf.RandRange(0, pavementFootsteps.Length);
//            PlayFootsteps(pavementFootsteps[randomIndex]);
//        }
//    }

//    public void PlayRandomMetalPipesFootstep()
//    {
//        if (metalpipesFootsteps.Length > 0)
//        {
//            int randomIndex = Mathf.RandRange(0, metalpipesFootsteps.Length);
//            PlayFootsteps(metalpipesFootsteps[randomIndex]);
//        }
//    }

//    public void StopFootsteps()
//    {
//        if (FootstepsSource.isPlaying)
//        {
//            FootstepsSource.Stop();       
//        }
//    }
//}
