using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace SampleCharacterSDK.Domain
{
    public class Character : MonoBehaviour, ICharacter
    {
        [SerializeField] private GameObject _character;
    }
}
