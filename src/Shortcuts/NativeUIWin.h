#pragma once
#define NOMINMAX
#include "NativeUI.h"
#include "Version.h"
#include <msclr/marshal_cppstd.h>
#include <algorithm>

struct NativeUI;

namespace Shortcuts {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
  using namespace System::Resources;
  using namespace System::Reflection;

	/// <summary>
	/// Summary for NativeUI
	/// </summary>
	public ref class NativeUIWin : public System::Windows::Forms::Form
	{
	public:
    NativeUIWin(NativeUI& callbacks):
      callbacks(callbacks)
		{
			InitializeComponent();
			
      ResourceManager^ rm = gcnew ResourceManager("Shortcuts.Resource", Assembly::GetExecutingAssembly());
      System::Drawing::Bitmap^ appBmp = (System::Drawing::Bitmap^) rm->GetObject("icon_512x512");
      this->Icon = System::Drawing::Icon::FromHandle(appBmp->GetHicon());

      System::Drawing::Bitmap^ trayBmp = (System::Drawing::Bitmap^) rm->GetObject("icon_16x16");
      this->notificationIcon->Icon = System::Drawing::Icon::FromHandle(trayBmp->GetHicon());

      this->label_version->Text = String::Concat(L"Shortcuts version:", Shortcuts_WVERSION);
		}

    static size_t s_nativeUIInitCount = 0;
    NativeUI& callbacks;

  public:



  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
  private: System::Windows::Forms::CheckBox^  mediaCheckBox;
  private: System::Windows::Forms::CheckBox^  exposeCheckBox;
  private: System::Windows::Forms::CheckBox^  scrollCheckBox;
  private: System::Windows::Forms::GroupBox^  box_featureSelection;


  private: System::Windows::Forms::TrackBar^  scrollSensitivityBar;

  private: System::Windows::Forms::Label^  label_scrollSensitivity;

  private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
  private: System::Windows::Forms::Button^  button_ok;

  private: System::Windows::Forms::Label^  label_version;
  private: System::Windows::Forms::CheckBox^  startupCheckBox;
  private: System::Windows::Forms::ToolStripMenuItem^  pauseToolStripMenuItem;






  public:




  public:

  public:
    static NativeUIWin^ s_nativeUI;

    static void AddTrayIcon(NativeUI& callbacks) {
      if (!s_nativeUIInitCount++)
      {
        s_nativeUI = gcnew NativeUIWin(callbacks);

        String^ appData = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
        String^ newDir = System::IO::Path::Combine(appData, L"Leap Motion\\Shortcuts");
        if( !System::IO::Directory::Exists(newDir) )
          System::IO::Directory::CreateDirectory(newDir);

        String^ configFile = System::IO::Path::Combine(newDir, L"config.json");
        msclr::interop::marshal_context ctxt;
        std::string str = ctxt.marshal_as<std::string>(configFile);
        callbacks.SetUserConfigFile(str);
        callbacks.RequestConfigs();
        s_nativeUI->startupCheckBox->Checked = callbacks.GetLaunchOnStartup();
      }
        
    }

    static void RemoveTrayIcon(void) {
      if(--s_nativeUIInitCount)
        return;

      s_nativeUI->Close();
      s_nativeUI = nullptr;
    }

    static void ConfigChanged(const std::string& var, bool value){
      if (!s_nativeUI)
        return;

      if (var == "enableScroll")
        s_nativeUI->scrollCheckBox->Checked = value;
      else if (var == "enableWindowSelection")
        s_nativeUI->exposeCheckBox->Checked = value;
      else if (var == "enableMedia")
        s_nativeUI->mediaCheckBox->Checked = value;
      else if (var == "showHelpOnStart") {
        if (value) {
          s_nativeUI->callbacks.OnShowHtmlHelp("main");
          s_nativeUI->callbacks.OnSettingChanged("showHelpOnStart", false);
        }
      }
    }

    static void ConfigChanged(const std::string& var, double value) {
      if(!s_nativeUI)
        return;
        
      if (var == "scrollSensitivity") {
        const int intvalue = std::max(s_nativeUI->scrollSensitivityBar->Minimum, std::min(s_nativeUI->scrollSensitivityBar->Maximum, static_cast<int>(value)));
        s_nativeUI->scrollSensitivityBar->Value = intvalue;
      }
    }
    
    static void ShowToolbarMessage(const char* title, const char* message) {
      if (!s_nativeUI)
        return;

      s_nativeUI->notificationIcon->ShowBalloonTip(10000, gcnew String(title), gcnew String(message), Windows::Forms::ToolTipIcon::Warning);
    }

    static void StartupChanged(bool value) {
      if (!s_nativeUI)
        return;
      s_nativeUI->startupCheckBox->Checked = value;
    }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
    ~NativeUIWin()
		{
      if(components)
        delete components;
      if(notificationIcon)
        delete notificationIcon;
		}

  private: System::Windows::Forms::NotifyIcon^  notificationIcon;
  private: System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
      this->components = (gcnew System::ComponentModel::Container());
      System::Windows::Forms::ContextMenuStrip^  notificationMenu;
      System::Windows::Forms::ToolStripMenuItem^  configToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
      this->pauseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->notificationIcon = (gcnew System::Windows::Forms::NotifyIcon(this->components));
      this->mediaCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->exposeCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->scrollCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->box_featureSelection = (gcnew System::Windows::Forms::GroupBox());
      this->scrollSensitivityBar = (gcnew System::Windows::Forms::TrackBar());
      this->label_scrollSensitivity = (gcnew System::Windows::Forms::Label());
      this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
      this->button_ok = (gcnew System::Windows::Forms::Button());
      this->label_version = (gcnew System::Windows::Forms::Label());
      this->startupCheckBox = (gcnew System::Windows::Forms::CheckBox());
      notificationMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
      configToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      notificationMenu->SuspendLayout();
      this->box_featureSelection->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->scrollSensitivityBar))->BeginInit();
      this->tableLayoutPanel1->SuspendLayout();
      this->SuspendLayout();
      // 
      // notificationMenu
      // 
      notificationMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
        this->pauseToolStripMenuItem,
          configToolStripMenuItem, helpToolStripMenuItem, this->toolStripSeparator1, exitToolStripMenuItem
      });
      notificationMenu->Name = L"contextMenuStrip1";
      notificationMenu->ShowCheckMargin = true;
      notificationMenu->ShowImageMargin = false;
      notificationMenu->Size = System::Drawing::Size(159, 120);
      // 
      // pauseToolStripMenuItem
      // 
      this->pauseToolStripMenuItem->Checked = true;
      this->pauseToolStripMenuItem->CheckOnClick = true;
      this->pauseToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
      this->pauseToolStripMenuItem->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->pauseToolStripMenuItem->Name = L"pauseToolStripMenuItem";
      this->pauseToolStripMenuItem->Size = System::Drawing::Size(133, 22);
      this->pauseToolStripMenuItem->Text = L"Pause Shortcuts";
      this->pauseToolStripMenuItem->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::pauseToolStripMenuItem_CheckedChanged);
      // 
      // configToolStripMenuItem
      // 
      configToolStripMenuItem->Name = L"configToolStripMenuItem";
      configToolStripMenuItem->Size = System::Drawing::Size(133, 22);
      configToolStripMenuItem->Text = L"&Configuration...";
      configToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::configToolStripMenuItem_Click);
      // 
      // helpToolStripMenuItem
      // 
      helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
      helpToolStripMenuItem->Size = System::Drawing::Size(133, 22);
      helpToolStripMenuItem->Text = L"&Help";
      helpToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::helpToolStripMenuItem_Click);
      // 
      // toolStripSeparator1
      // 
      this->toolStripSeparator1->Name = L"toolStripSeparator1";
      this->toolStripSeparator1->Size = System::Drawing::Size(130, 6);
      // 
      // exitToolStripMenuItem
      // 
      exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      exitToolStripMenuItem->Size = System::Drawing::Size(133, 22);
      exitToolStripMenuItem->Text = L"&Exit";
      exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::exitToolStripMenuItem_Click);
      // 
      // notificationIcon
      // 
      this->notificationIcon->ContextMenuStrip = notificationMenu;
      this->notificationIcon->Text = L"Shortcuts";
      this->notificationIcon->Visible = true;
      // 
      // mediaCheckBox
      // 
      this->mediaCheckBox->AutoSize = true;
      this->mediaCheckBox->Location = System::Drawing::Point(11, 77);
      this->mediaCheckBox->Name = L"mediaCheckBox";
      this->mediaCheckBox->Size = System::Drawing::Size(151, 19);
      this->mediaCheckBox->TabIndex = 5;
      this->mediaCheckBox->Text = L"Enable Media Controls";
      this->mediaCheckBox->UseVisualStyleBackColor = true;
      this->mediaCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::mediaCheckBox_CheckedChanged);
      // 
      // exposeCheckBox
      // 
      this->exposeCheckBox->AutoSize = true;
      this->exposeCheckBox->Location = System::Drawing::Point(11, 51);
      this->exposeCheckBox->Name = L"exposeCheckBox";
      this->exposeCheckBox->Size = System::Drawing::Size(178, 19);
      this->exposeCheckBox->TabIndex = 1;
      this->exposeCheckBox->Text = L"Enable Application Switcher";
      this->exposeCheckBox->UseVisualStyleBackColor = true;
      this->exposeCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::exposeCheckBox_CheckedChanged);
      // 
      // scrollCheckBox
      // 
      this->scrollCheckBox->AutoSize = true;
      this->scrollCheckBox->Location = System::Drawing::Point(11, 25);
      this->scrollCheckBox->Name = L"scrollCheckBox";
      this->scrollCheckBox->Size = System::Drawing::Size(116, 19);
      this->scrollCheckBox->TabIndex = 3;
      this->scrollCheckBox->Text = L"Enable Scrolling";
      this->scrollCheckBox->UseVisualStyleBackColor = true;
      this->scrollCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::scrollCheckBox_CheckedChanged);
      // 
      // box_featureSelection
      // 
      this->box_featureSelection->Controls->Add(this->exposeCheckBox);
      this->box_featureSelection->Controls->Add(this->scrollCheckBox);
      this->box_featureSelection->Controls->Add(this->mediaCheckBox);
      this->box_featureSelection->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9));
      this->box_featureSelection->Location = System::Drawing::Point(14, 13);
      this->box_featureSelection->Margin = System::Windows::Forms::Padding(2);
      this->box_featureSelection->Name = L"box_featureSelection";
      this->box_featureSelection->Padding = System::Windows::Forms::Padding(2);
      this->box_featureSelection->Size = System::Drawing::Size(190, 105);
      this->box_featureSelection->TabIndex = 6;
      this->box_featureSelection->TabStop = false;
      this->box_featureSelection->Text = L"Feature Selection";
      // 
      // scrollSensitivityBar
      // 
      this->scrollSensitivityBar->LargeChange = 1;
      this->scrollSensitivityBar->Location = System::Drawing::Point(2, 2);
      this->scrollSensitivityBar->Margin = System::Windows::Forms::Padding(2);
      this->scrollSensitivityBar->Maximum = 9;
      this->scrollSensitivityBar->Minimum = 1;
      this->scrollSensitivityBar->Name = L"scrollSensitivityBar";
      this->scrollSensitivityBar->Size = System::Drawing::Size(232, 26);
      this->scrollSensitivityBar->TabIndex = 6;
      this->scrollSensitivityBar->Value = 5;
      this->scrollSensitivityBar->MouseCaptureChanged += gcnew System::EventHandler(this, &NativeUIWin::trackBar1_MouseCaptureChanged);
      // 
      // label_scrollSensitivity
      // 
      this->label_scrollSensitivity->Anchor = System::Windows::Forms::AnchorStyles::Top;
      this->label_scrollSensitivity->AutoSize = true;
      this->label_scrollSensitivity->Location = System::Drawing::Point(76, 30);
      this->label_scrollSensitivity->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label_scrollSensitivity->Name = L"label_scrollSensitivity";
      this->label_scrollSensitivity->Size = System::Drawing::Size(83, 13);
      this->label_scrollSensitivity->TabIndex = 7;
      this->label_scrollSensitivity->Text = L"Scroll Sensitivity";
      // 
      // tableLayoutPanel1
      // 
      this->tableLayoutPanel1->ColumnCount = 1;
      this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
        50)));
      this->tableLayoutPanel1->Controls->Add(this->scrollSensitivityBar, 0, 0);
      this->tableLayoutPanel1->Controls->Add(this->label_scrollSensitivity, 0, 1);
      this->tableLayoutPanel1->Location = System::Drawing::Point(220, 21);
      this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(2);
      this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
      this->tableLayoutPanel1->RowCount = 2;
      this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
      this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 19)));
      this->tableLayoutPanel1->Size = System::Drawing::Size(236, 49);
      this->tableLayoutPanel1->TabIndex = 8;
      // 
      // button_ok
      // 
      this->button_ok->Location = System::Drawing::Point(387, 121);
      this->button_ok->Margin = System::Windows::Forms::Padding(2);
      this->button_ok->Name = L"button_ok";
      this->button_ok->Size = System::Drawing::Size(69, 24);
      this->button_ok->TabIndex = 9;
      this->button_ok->Text = L"Close";
      this->button_ok->UseVisualStyleBackColor = true;
      this->button_ok->Click += gcnew System::EventHandler(this, &NativeUIWin::okButton_Click);
      // 
      // label_version
      // 
      this->label_version->AutoSize = true;
      this->label_version->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7));
      this->label_version->Location = System::Drawing::Point(217, 128);
      this->label_version->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label_version->Name = L"label_version";
      this->label_version->Size = System::Drawing::Size(91, 13);
      this->label_version->TabIndex = 10;
      this->label_version->Text = L"Shortcuts version:";
      // 
      // startupCheckBox
      // 
      this->startupCheckBox->AutoSize = true;
      this->startupCheckBox->Location = System::Drawing::Point(25, 126);
      this->startupCheckBox->Name = L"startupCheckBox";
      this->startupCheckBox->Size = System::Drawing::Size(162, 17);
      this->startupCheckBox->TabIndex = 6;
      this->startupCheckBox->Text = L"Launch Shortcuts on Startup";
      this->startupCheckBox->UseVisualStyleBackColor = true;
      this->startupCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::startupCheckBox_CheckedChanged);
      // 
      // NativeUIWin
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->AutoSize = true;
      this->ClientSize = System::Drawing::Size(467, 157);
      this->Controls->Add(this->startupCheckBox);
      this->Controls->Add(this->label_version);
      this->Controls->Add(this->button_ok);
      this->Controls->Add(this->tableLayoutPanel1);
      this->Controls->Add(this->box_featureSelection);
      this->MaximizeBox = false;
      this->MinimizeBox = false;
      this->Name = L"NativeUIWin";
      this->Text = L"Shortcuts Configuration";
      this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &NativeUIWin::NativeUIWin_FormClosing);
      notificationMenu->ResumeLayout(false);
      this->box_featureSelection->ResumeLayout(false);
      this->box_featureSelection->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->scrollSensitivityBar))->EndInit();
      this->tableLayoutPanel1->ResumeLayout(false);
      this->tableLayoutPanel1->PerformLayout();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  void exitToolStripMenuItem_Click(Object^  sender, System::EventArgs^  e) {
    callbacks.OnQuit();
    Close();
  }
  
  void configToolStripMenuItem_Click(Object^  sender, System::EventArgs^  e) {
    Visible = true;
  }

  void NativeUIWin_FormClosing(Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
    switch(e->CloseReason) {
    case CloseReason::UserClosing:
      callbacks.OnConfigUiHidden(true);
      e->Cancel = true;
      Visible = false;
      break;
    default:
      // If the close reason is anything else, we're getting terminated with good cause.  Instead of
      // treating this as a simple "config dismissed" event, we should instead treat it as an "app
      // quit" event.
      callbacks.OnQuit();
      break;
    }
  }
  System::Void okButton_Click(Object^  sender, System::EventArgs^  e) {
    callbacks.OnConfigUiHidden(false);
    Visible = false;
  }
  System::Void cancelButton_Click(Object^  sender, System::EventArgs^  e) {
    callbacks.OnConfigUiHidden(true);
    Visible = false;
  }
  System::Void helpToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnShowHtmlHelp("main");
  }

  System::Void scrollCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("enableScroll", scrollCheckBox->Checked);
  }
  
  System::Void exposeCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("enableWindowSelection", exposeCheckBox->Checked);
  }
  
  System::Void mediaCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("enableMedia", mediaCheckBox->Checked);
  }
  System::Void trackBar1_MouseCaptureChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("scrollSensitivity", (double)scrollSensitivityBar->Value);
  }
  System::Void startupCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnStartupChanged(startupCheckBox->Checked);
  }
  System::Void pauseToolStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnPauseInteraction(pauseToolStripMenuItem->Checked);
  }

};
}
