using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MusicEffect : MonoBehaviour
{
    public AudioClip music;
    private AudioSource _audioSource;
    private float _musicLength;
    private float elapsedTime = 0;

    // Start is called before the first frame update
    void Start()
    {
        _audioSource = this.gameObject.AddComponent<AudioSource>();
        _audioSource.loop = false;
        _audioSource.volume = 1.0f;
        _audioSource.clip = music;
        _audioSource.Play();
        _musicLength = music.length * 2;
    }

    // Update is called once per frame
    void Update()
    {
        elapsedTime += Time.deltaTime;
        if (elapsedTime >= _musicLength) {
            Destroy(gameObject);
        }
    }
}
