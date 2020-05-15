using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class IntroMusic : MonoBehaviour
{
    public AudioClip music;
    private AudioSource _audioSource;

    // Start is called before the first frame update
    void Start()
    {
        _audioSource = this.gameObject.AddComponent<AudioSource>();
        _audioSource.loop = true;
        _audioSource.volume = 1.0f;
        _audioSource.clip = music;
        _audioSource.Play();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
