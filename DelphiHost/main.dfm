object fmMain: TfmMain
  Left = 208
  Top = 348
  Width = 917
  Height = 456
  Caption = 'FreeFrame Delphi Host Test Container'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PaintBox1: TPaintBox
    Left = 152
    Top = 128
    Width = 280
    Height = 210
  end
  object lTestContainerVersion: TLabel
    Left = 15
    Top = 397
    Width = 34
    Height = 13
    Caption = 'version'
  end
  object lInitPlugin: TLabel
    Left = 496
    Top = 136
    Width = 25
    Height = 13
    Caption = 'result'
  end
  object lDeInitPlugin: TLabel
    Left = 496
    Top = 350
    Width = 25
    Height = 13
    Caption = 'result'
  end
  object lGetPluginInfo: TLabel
    Left = 496
    Top = 85
    Width = 25
    Height = 13
    Caption = 'result'
  end
  object lProcessFrame: TLabel
    Left = 496
    Top = 185
    Width = 25
    Height = 13
    Caption = 'result'
  end
  object Label1: TLabel
    Left = 448
    Top = 5
    Width = 62
    Height = 13
    Caption = 'Select Plugin'
  end
  object lNumParameters: TLabel
    Left = 604
    Top = 202
    Width = 60
    Height = 13
    Caption = 'Num Params'
  end
  object lParam0Name: TLabel
    Left = 584
    Top = 222
    Width = 64
    Height = 13
    Caption = 'Param0Name'
  end
  object lParam1Name: TLabel
    Left = 584
    Top = 273
    Width = 64
    Height = 13
    Caption = 'Param1Name'
  end
  object lParam2Name: TLabel
    Left = 584
    Top = 323
    Width = 64
    Height = 13
    Caption = 'Param2Name'
  end
  object lParam3Name: TLabel
    Left = 584
    Top = 371
    Width = 64
    Height = 13
    Caption = 'Param3Name'
  end
  object lParam0Value: TLabel
    Left = 732
    Top = 233
    Width = 63
    Height = 13
    Caption = 'Param0Value'
  end
  object lParam1Value: TLabel
    Left = 732
    Top = 284
    Width = 63
    Height = 13
    Caption = 'Param1Value'
  end
  object lParam2Value: TLabel
    Left = 732
    Top = 334
    Width = 63
    Height = 13
    Caption = 'Param2Value'
  end
  object lParam3Value: TLabel
    Left = 732
    Top = 382
    Width = 63
    Height = 13
    Caption = 'Param3Value'
  end
  object lParam0dword: TLabel
    Left = 733
    Top = 249
    Width = 67
    Height = 13
    Caption = 'Param0Dword'
  end
  object lParam1dword: TLabel
    Left = 733
    Top = 300
    Width = 67
    Height = 13
    Caption = 'Param1Dword'
  end
  object lParam2dword: TLabel
    Left = 733
    Top = 350
    Width = 67
    Height = 13
    Caption = 'Param2Dword'
  end
  object lParam3dword: TLabel
    Left = 733
    Top = 398
    Width = 67
    Height = 13
    Caption = 'Param3Dword'
  end
  object Label2: TLabel
    Left = 80
    Top = 376
    Width = 350
    Height = 13
    Caption = 
      'Plugins must be in a plugins subdirectory of the  directory wher' +
      'e this .exe is'
  end
  object lProfile: TLabel
    Left = 472
    Top = 392
    Width = 29
    Height = 13
    Caption = 'Profile'
  end
  object Label3: TLabel
    Left = 456
    Top = 376
    Width = 108
    Height = 13
    Caption = 'Plugin processing time:'
  end
  object bInit: TButton
    Left = 48
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Init AVI'
    TabOrder = 1
    OnClick = bInitClick
  end
  object bDeInit: TButton
    Left = 48
    Top = 280
    Width = 75
    Height = 25
    Caption = 'DeInit AVI'
    TabOrder = 16
    OnClick = bDeInitClick
  end
  object bOpenAVI: TButton
    Left = 48
    Top = 94
    Width = 75
    Height = 25
    Caption = 'Open AVI'
    TabOrder = 2
    OnClick = bOpenAVIClick
  end
  object ebAVIFilename: TEdit
    Left = 144
    Top = 97
    Width = 289
    Height = 21
    TabOrder = 18
    Text = 'C:\gig av\03Lavalamp A\gloup1bottomviewx.avi'
    OnChange = ebAVIFilenameChange
  end
  object bCloseAVI: TButton
    Left = 48
    Top = 218
    Width = 75
    Height = 25
    Caption = 'Close AVI'
    TabOrder = 15
    OnClick = bCloseAVIClick
  end
  object bGetFrame: TButton
    Left = 48
    Top = 156
    Width = 75
    Height = 25
    Caption = 'Get Frame'
    TabOrder = 3
    OnClick = bGetFrameClick
  end
  object bgetInfo: TButton
    Left = 463
    Top = 60
    Width = 91
    Height = 25
    Caption = 'Plugin getInfo'
    TabOrder = 4
    OnClick = bgetInfoClick
  end
  object GroupBox1: TGroupBox
    Left = 600
    Top = 8
    Width = 113
    Height = 105
    Caption = ' PluginInfoStruct '
    TabOrder = 19
    object lPluginMajorVersion: TLabel
      Left = 9
      Top = 19
      Width = 63
      Height = 13
      Caption = 'Major version'
    end
    object lPluginMinorVersion: TLabel
      Left = 9
      Top = 35
      Width = 63
      Height = 13
      Caption = 'Minor version'
    end
    object lPluginName: TLabel
      Left = 9
      Top = 66
      Width = 28
      Height = 13
      Caption = 'Name'
    end
    object lPluginType: TLabel
      Left = 9
      Top = 81
      Width = 24
      Height = 13
      Caption = 'Type'
    end
    object lPluginUniqueID: TLabel
      Left = 9
      Top = 50
      Width = 48
      Height = 13
      Caption = 'Unique ID'
    end
  end
  object GroupBox2: TGroupBox
    Left = 256
    Top = 16
    Width = 113
    Height = 73
    Caption = ' VideoInfoStruct '
    TabOrder = 20
    object lVideoWidth: TLabel
      Left = 13
      Top = 16
      Width = 28
      Height = 13
      Caption = 'Width'
    end
    object lVideoHeight: TLabel
      Left = 13
      Top = 32
      Width = 31
      Height = 13
      Caption = 'Height'
    end
    object lBitDepth: TLabel
      Left = 13
      Top = 48
      Width = 44
      Height = 13
      Caption = 'Bit Depth'
    end
  end
  object bInitPlugin: TButton
    Left = 471
    Top = 108
    Width = 75
    Height = 25
    Caption = 'Init Plugin'
    TabOrder = 5
    OnClick = bInitPluginClick
  end
  object bProcessFrame: TButton
    Left = 464
    Top = 158
    Width = 89
    Height = 25
    Caption = 'Process Frame'
    TabOrder = 11
    OnClick = bProcessFrameClick
  end
  object bDeInitPlugin: TButton
    Left = 471
    Top = 320
    Width = 75
    Height = 25
    Caption = 'DeInit Plugin'
    TabOrder = 17
    OnClick = bDeInitPluginClick
  end
  object cbPlugins: TComboBox
    Left = 408
    Top = 20
    Width = 166
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 25
    OnChange = cbPluginsChange
  end
  object bGetNumParameters: TButton
    Left = 762
    Top = 21
    Width = 105
    Height = 25
    Caption = 'GetNumParameters'
    TabOrder = 6
    OnClick = bGetNumParametersClick
  end
  object tbParam0: TTrackBar
    Left = 576
    Top = 234
    Width = 150
    Height = 33
    Max = 100
    Orientation = trHorizontal
    Frequency = 10
    Position = 0
    SliderVisible = False
    SelEnd = 0
    SelStart = 0
    TabOrder = 21
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = tbParam0Change
  end
  object tbParam1: TTrackBar
    Left = 576
    Top = 285
    Width = 150
    Height = 33
    Max = 100
    Orientation = trHorizontal
    Frequency = 10
    Position = 0
    SliderVisible = False
    SelEnd = 0
    SelStart = 0
    TabOrder = 22
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = tbParam1Change
  end
  object tbParam2: TTrackBar
    Left = 576
    Top = 335
    Width = 150
    Height = 33
    Max = 100
    Orientation = trHorizontal
    Frequency = 10
    Position = 0
    SliderVisible = False
    SelEnd = 0
    SelStart = 0
    TabOrder = 23
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = tbParam2Change
  end
  object tbParam3: TTrackBar
    Left = 576
    Top = 383
    Width = 150
    Height = 33
    Max = 100
    Orientation = trHorizontal
    Frequency = 10
    Position = 0
    SliderVisible = False
    SelEnd = 0
    SelStart = 0
    TabOrder = 24
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = tbParam3Change
  end
  object bGetParameterNames: TButton
    Left = 762
    Top = 62
    Width = 104
    Height = 25
    Caption = 'Get Param Names'
    TabOrder = 7
    OnClick = bGetParameterNamesClick
  end
  object bGetParamDefaults: TButton
    Left = 762
    Top = 102
    Width = 104
    Height = 25
    Caption = 'Get Param Defaults'
    TabOrder = 8
    OnClick = bGetParamDefaultsClick
  end
  object bGetParamDisplayValues: TButton
    Left = 746
    Top = 143
    Width = 137
    Height = 25
    Caption = 'Get Param Display Values'
    TabOrder = 9
    OnClick = bGetParamDisplayValuesClick
  end
  object bGetParamActualValues: TButton
    Left = 746
    Top = 183
    Width = 137
    Height = 25
    Caption = 'Get Param Actual Values'
    TabOrder = 10
    OnClick = bGetParamActualValuesClick
  end
  object bGetAndProcess: TButton
    Left = 456
    Top = 208
    Width = 105
    Height = 25
    Caption = 'Get and Process'
    TabOrder = 12
    OnClick = bGetAndProcessClick
  end
  object bBrowse: TButton
    Left = 152
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Browse'
    TabOrder = 0
    OnClick = bBrowseClick
  end
  object gbPluginCaps: TGroupBox
    Left = 600
    Top = 121
    Width = 113
    Height = 73
    Caption = ' Plugin Capabilities '
    TabOrder = 26
    object l16bit: TLabel
      Left = 9
      Top = 19
      Width = 26
      Height = 13
      Caption = '16 bit'
    end
    object l24bit: TLabel
      Left = 9
      Top = 35
      Width = 26
      Height = 13
      Caption = '24 bit'
    end
    object l32bit: TLabel
      Left = 9
      Top = 50
      Width = 26
      Height = 13
      Caption = '32 bit'
    end
  end
  object bPlayAndProcess: TButton
    Left = 456
    Top = 245
    Width = 105
    Height = 25
    Caption = 'Play and Process'
    TabOrder = 13
    OnClick = bPlayAndProcessClick
  end
  object bStop: TButton
    Left = 472
    Top = 281
    Width = 75
    Height = 25
    Caption = 'Stop'
    TabOrder = 14
    OnClick = bStopClick
  end
  object odAVI: TOpenDialog
    Left = 392
    Top = 40
  end
  object tPlay: TTimer
    Enabled = False
    Interval = 40
    OnTimer = tPlayTimer
    Left = 176
    Top = 24
  end
end
