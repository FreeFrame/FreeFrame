object fmMain: TfmMain
  Left = 80
  Top = 156
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  BorderStyle = bsSingle
  Caption = 'FreeFrame Delphi Host Test Container'
  ClientHeight = 304
  ClientWidth = 804
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
  object Shape1: TShape
    Left = 3
    Top = 62
    Width = 320
    Height = 240
    Brush.Color = clBlack
  end
  object PaintBox1: TPaintBox
    Left = 3
    Top = 62
    Width = 320
    Height = 240
    Color = clBlack
    ParentColor = False
    OnMouseMove = PaintBox1MouseMove
  end
  object lParam0Name: TLabel
    Left = 449
    Top = 89
    Width = 3
    Height = 13
  end
  object lParam1Name: TLabel
    Left = 449
    Top = 140
    Width = 3
    Height = 13
  end
  object lParam2Name: TLabel
    Left = 449
    Top = 190
    Width = 3
    Height = 13
  end
  object lParam3Name: TLabel
    Left = 449
    Top = 238
    Width = 3
    Height = 13
  end
  object lParam0Value: TLabel
    Left = 597
    Top = 100
    Width = 63
    Height = 13
    AutoSize = False
  end
  object lParam1Value: TLabel
    Left = 597
    Top = 151
    Width = 63
    Height = 13
    AutoSize = False
  end
  object lParam2Value: TLabel
    Left = 597
    Top = 201
    Width = 63
    Height = 13
    AutoSize = False
  end
  object lParam3Value: TLabel
    Left = 597
    Top = 249
    Width = 63
    Height = 13
    AutoSize = False
  end
  object lParam0dword: TLabel
    Left = 598
    Top = 116
    Width = 67
    Height = 13
    AutoSize = False
  end
  object lParam1dword: TLabel
    Left = 598
    Top = 167
    Width = 67
    Height = 13
    AutoSize = False
  end
  object lParam2dword: TLabel
    Left = 598
    Top = 217
    Width = 67
    Height = 13
    AutoSize = False
  end
  object lParam3dword: TLabel
    Left = 598
    Top = 265
    Width = 67
    Height = 13
    AutoSize = False
  end
  object lbEffect1: TLabel
    Left = 449
    Top = 4
    Width = 39
    Height = 13
    Caption = 'effect 1:'
  end
  object lbEffect2: TLabel
    Left = 449
    Top = 47
    Width = 39
    Height = 13
    Caption = 'effect 2:'
  end
  object bInit: TButton
    Left = 82
    Top = 2
    Width = 76
    Height = 25
    Caption = '1 - Init AVI'
    TabOrder = 1
    OnClick = bInitClick
  end
  object bOpenAVI: TButton
    Left = 2
    Top = 32
    Width = 75
    Height = 25
    Caption = '2 - Open AVI'
    TabOrder = 2
    OnClick = bOpenAVIClick
  end
  object ebAVIFilename: TEdit
    Left = 163
    Top = 5
    Width = 120
    Height = 21
    TabOrder = 5
    OnChange = ebAVIFilenameChange
  end
  object GroupBox1: TGroupBox
    Left = 328
    Top = 2
    Width = 113
    Height = 103
    Caption = ' PluginInfoStruct '
    TabOrder = 6
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
    Left = 328
    Top = 176
    Width = 113
    Height = 74
    Caption = ' VideoInfoStruct '
    TabOrder = 7
    object lVideoWidth: TLabel
      Left = 10
      Top = 19
      Width = 28
      Height = 13
      Caption = 'Width'
    end
    object lVideoHeight: TLabel
      Left = 10
      Top = 35
      Width = 31
      Height = 13
      Caption = 'Height'
    end
    object lBitDepth: TLabel
      Left = 10
      Top = 51
      Width = 44
      Height = 13
      Caption = 'Bit Depth'
    end
  end
  object tbParam0: TTrackBar
    Left = 441
    Top = 101
    Width = 150
    Height = 33
    Max = 100
    Frequency = 10
    TabOrder = 8
    TickMarks = tmBoth
    TickStyle = tsNone
    Visible = False
    OnChange = tbParam0Change
  end
  object tbParam1: TTrackBar
    Left = 441
    Top = 152
    Width = 150
    Height = 33
    Max = 100
    Frequency = 10
    TabOrder = 9
    TickMarks = tmBoth
    TickStyle = tsNone
    Visible = False
    OnChange = tbParam1Change
  end
  object tbParam2: TTrackBar
    Left = 441
    Top = 202
    Width = 150
    Height = 33
    Max = 100
    Frequency = 10
    TabOrder = 10
    TickMarks = tmBoth
    TickStyle = tsNone
    Visible = False
    OnChange = tbParam2Change
  end
  object tbParam3: TTrackBar
    Left = 441
    Top = 250
    Width = 150
    Height = 33
    Max = 100
    Frequency = 10
    TabOrder = 11
    TickMarks = tmBoth
    TickStyle = tsNone
    Visible = False
    OnChange = tbParam3Change
  end
  object bBrowse: TButton
    Left = 2
    Top = 2
    Width = 75
    Height = 25
    Caption = '0 - Browse'
    TabOrder = 0
    OnClick = bBrowseClick
  end
  object gbPluginCaps: TGroupBox
    Left = 328
    Top = 106
    Width = 113
    Height = 69
    Caption = ' Plugin Capabilities '
    TabOrder = 12
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
    Left = 82
    Top = 32
    Width = 76
    Height = 25
    Caption = '3 - Play '
    TabOrder = 3
    OnClick = bPlayAndProcessClick
  end
  object bStop: TButton
    Left = 164
    Top = 32
    Width = 75
    Height = 25
    Caption = '4 - Stop'
    TabOrder = 4
    OnClick = bStopClick
  end
  object cbEffect1Active: TCheckBox
    Left = 547
    Top = 18
    Width = 17
    Height = 15
    Checked = True
    State = cbChecked
    TabOrder = 13
  end
  object cbEffect2Active: TCheckBox
    Left = 547
    Top = 60
    Width = 25
    Height = 17
    Checked = True
    State = cbChecked
    TabOrder = 14
  end
  object lbPlugins: TListBox
    Left = 664
    Top = 3
    Width = 137
    Height = 299
    DragMode = dmAutomatic
    ItemHeight = 13
    TabOrder = 15
  end
  object pPlugin1: TPanel
    Tag = 1
    Left = 448
    Top = 19
    Width = 97
    Height = 25
    BevelOuter = bvNone
    BorderWidth = 1
    BorderStyle = bsSingle
    Caption = '[none]'
    Ctl3D = False
    ParentCtl3D = False
    TabOrder = 16
    OnClick = pPlugin1Click
    OnDragDrop = pPluginDragDrop
    OnDragOver = pPluginDragOver
  end
  object Panel1: TPanel
    Tag = 2
    Left = 448
    Top = 62
    Width = 97
    Height = 25
    BevelOuter = bvNone
    BorderWidth = 1
    BorderStyle = bsSingle
    Caption = '[none]'
    Ctl3D = False
    ParentCtl3D = False
    TabOrder = 17
    OnClick = Panel1Click
    OnDragDrop = pPluginDragDrop
    OnDragOver = pPluginDragOver
  end
  object rgMouse0: TRadioGroup
    Left = 328
    Top = 251
    Width = 114
    Height = 51
    Caption = 'mouse coord.'
    Columns = 2
    ItemIndex = 0
    Items.Strings = (
      'none'
      'x'
      'y'
      'x/y')
    TabOrder = 18
  end
  object odAVI: TOpenDialog
    Filter = '*.avi'
    Left = 40
    Top = 64
  end
  object tPlay: TTimer
    Enabled = False
    Interval = 40
    OnTimer = tPlayTimer
    Left = 8
    Top = 64
  end
  object FreeFrame1: TFreeFrame
    PluginIndex = 0
    Autoloadplugin = False
    NumParameters = 0
    Left = 72
    Top = 64
  end
  object FreeFrame2: TFreeFrame
    PluginIndex = 0
    Autoloadplugin = False
    NumParameters = 0
    Left = 104
    Top = 64
  end
end
