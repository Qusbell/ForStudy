using SampleApp.Linq;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using UniRx;
using UnityEngine;

namespace SampleApp.View
{
    public class ObservableDropdownTMPro : ObservableDropdown
    {

        const string INDETERMINATE_VALUE = "---";

        [SerializeField] private TMP_Dropdown m_Dropdown = null;
        [SerializeField] private TMP_Text m_PlaceHolder = null;
        [SerializeField] private float _optionWidthOffset = 50;

        public override int Value { get => m_Dropdown.value; set => m_Dropdown.value = value; }
        public override IObservable<int> OnValueChanged => m_Dropdown.onValueChanged.AsObservable();

        public override bool Interactable { get => m_Dropdown.interactable; set => m_Dropdown.interactable = value; }

        public override string Text
        {
            get
            {
                if(m_Dropdown.value < 0)
                {
                    return INDETERMINATE_VALUE;
                }

                var idx = m_Dropdown.value;
                return m_Dropdown.options[idx].text;
            }
            set
            {
                int index = m_Dropdown.options.IndexOf( opt => opt.text == value );
                m_Dropdown.value = index;
            }
        }

        public override IObservable<string> OnTextChanged => OnValueChanged.Select( index => m_Dropdown.options[index].text );

        public override List<string> Options
        {
            get
            {
                return m_Dropdown.options
                    .Select( option => option.text )
                    .ToList();
            }
        }

        private void Awake()
        {
            Debug.Assert( m_Dropdown.placeholder != null, this );
            Debug.Assert( m_Dropdown.placeholder == m_PlaceHolder, this );

            m_PlaceHolder.text = INDETERMINATE_VALUE;

            m_Dropdown.onValueChanged
               .AsObservable()
               .Select( index => index);

            OnValueChanged.Select( index => m_Dropdown.options[index].text );

            m_Dropdown.template.anchorMin = new Vector2( 1, 0 );
            m_Dropdown.template.anchorMax = new Vector2( 1, 0 );
        }

        private void OnEnable()
        {
            UpdateTemplateWidth();
        }

        public override void SetOptions( IReadOnlyList<string> options )
        {
            if(m_Dropdown.value >= m_Dropdown.options.Count)
            {
                m_Dropdown.options = options
                    .Select( str => new TMP_Dropdown.OptionData( str ) )
                    .ToList();
            }
            else
            {
                var newIndex = options.IndexOf( opt => opt == Text );
                m_Dropdown.options = options
                    .Select( str => new TMP_Dropdown.OptionData( str ) )
                    .ToList();
                SetValueWithoutNotify( newIndex );
            }

            UpdateTemplateWidth();
        }

        public override void SetValueWithoutNotify( int value )
        {
            if(value != m_Dropdown.value)
            {
                m_Dropdown.SetValueWithoutNotify( value );
            }
            else
            {
                m_Dropdown.RefreshShownValue();
            }
        }

        public override void SetTextWithoutNotify( string text )
        {
            int index = m_Dropdown.options.IndexOf(opt => opt.text == text);
            SetValueWithoutNotify( index );
        }

        private void UpdateTemplateWidth()
        {
            float widthDelta = (m_Dropdown.transform as RectTransform).rect.width;
            string maxLenText = string.Empty;
            for(int i = 0; i < m_Dropdown.options.Count; ++i)
            {
                var opt = m_Dropdown.options[i];
                if(maxLenText.Length < opt.text.Length)
                {
                    maxLenText = opt.text;
                }
            }
            var pref = m_Dropdown.itemText.GetPreferredValues( maxLenText );
            widthDelta = Mathf.Max( pref.x + _optionWidthOffset, widthDelta );

            var sizeDelta = m_Dropdown.template.sizeDelta;
            sizeDelta.x = widthDelta;
            m_Dropdown.template.sizeDelta = sizeDelta;
        }

        public override void SetIndeterminateValue()
        {
            m_Dropdown.SetValueWithoutNotify( -1 );
        }
    }
}
