using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace SampleApp.View
{
    public abstract class ObservableDropdown : MonoBehaviour
    {
        public abstract int Value { get; set; }
        public abstract IObservable<int> OnValueChanged { get; }
        public abstract void SetValueWithoutNotify( int value );

        public abstract string Text { get; set; }
        public abstract IObservable<string> OnTextChanged { get; }
        public abstract void SetTextWithoutNotify( string text );

        public abstract void SetIndeterminateValue();

        public abstract List<string> Options { get; }
        public abstract void SetOptions( IReadOnlyList<string> options );

        public abstract bool Interactable { get; set; }
    }
}
