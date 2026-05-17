using SampleApp.Presenter;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace SampleApp.View
{
    public class SampleAppUIView : MonoBehaviour, ISampleAppUIView
    {
        [SerializeField] private ObservableDropdown _dropDown;

        public IObservable<int> OnSelectIndexChange => _dropDown.OnValueChanged;

        public IObservable<string> OnSelectNameChange => _dropDown.OnTextChanged;

        public void SetOptions( IReadOnlyList<string> options )
        {
            _dropDown.Options.Clear();
            _dropDown.SetOptions( options );
        }
    }
}
