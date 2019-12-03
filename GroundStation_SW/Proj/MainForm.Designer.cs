/*
 * Created by SharpDevelop.
 * User: Graham
 * Date: 27/09/2014
 * Time: 5:03 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace cansat
{
	partial class MainForm
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		private System.IO.Ports.SerialPort serialPort1;
		private System.Windows.Forms.ComboBox PortDropDown;
		private System.Windows.Forms.Label label_port;
		private System.Windows.Forms.Button button_open;
		private System.Windows.Forms.Label label_id_display;
		private System.Windows.Forms.Label label_time_display;
		private System.Windows.Forms.Label label_alt_display;
		//private System.Windows.Forms.Label label_outside_display;
		private System.Windows.Forms.Label label_inside_display;
		private System.Windows.Forms.Label label_voltage_display;
		private System.Windows.Forms.Label label_bonus_display;
		private System.Windows.Forms.Label label_state_display;
		private System.Windows.Forms.Label label_outside_temp_display;
		private System.Windows.Forms.DataVisualization.Charting.Chart chartSummary;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label labelState;
		private System.Windows.Forms.TabControl tabControl;
		private System.Windows.Forms.TabPage tabSummary;
		private System.Windows.Forms.TabPage tabAltitude;
		private System.Windows.Forms.DataVisualization.Charting.Chart chartAltitude;
		private System.Windows.Forms.TabPage tabVoltage;
		private System.Windows.Forms.DataVisualization.Charting.Chart chartVoltage;
		private System.Windows.Forms.TabPage tabOutsideTemp;
		private System.Windows.Forms.DataVisualization.Charting.Chart chartOutsideTemp;
		private System.Windows.Forms.TabPage tabInsideTemp;
		private System.Windows.Forms.DataVisualization.Charting.Chart chartInsideTemp;
		//er;
		private System.Windows.Forms.DataVisualization.Charting.Chart chartAccel;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TabPage tabAccelerometer;
		private System.Windows.Forms.TabPage tabConfig;
		//private System.Windows.Forms.ComboBox comboBox1;
		//private System.Windows.Forms.TabPage tabConfig;
		//private System.Windows.Forms.ComboBox comboBox1;
		private System.Windows.Forms.ComboBox baudDropDown;
		private System.Windows.Forms.RichTextBox richTextBox1;
		private System.Windows.Forms.TabPage tab_packets;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Button button_cal;
		private System.Windows.Forms.Button button3;
		private System.Windows.Forms.Button clear_data_button;
		//private System.Windows.Forms.TabPage tab_packets;

		//private System.Windows.Forms.Label label_voltage_display;
		
		/// <summary>
		/// Disposes resources used by the form.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}
		
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		//private void InitializeComponent()
		
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea4 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea5 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea6 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea7 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.Series series6 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.Series series7 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea8 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Series series8 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea9 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Series series9 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea10 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Series series10 = new System.Windows.Forms.DataVisualization.Charting.Series();
			System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea11 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
			System.Windows.Forms.DataVisualization.Charting.Series series11 = new System.Windows.Forms.DataVisualization.Charting.Series();
			this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
			this.PortDropDown = new System.Windows.Forms.ComboBox();
			this.label_port = new System.Windows.Forms.Label();
			this.button_open = new System.Windows.Forms.Button();
			this.label_id_display = new System.Windows.Forms.Label();
			this.label_time_display = new System.Windows.Forms.Label();
			this.label_alt_display = new System.Windows.Forms.Label();
			this.label_outside_temp_display = new System.Windows.Forms.Label();
			this.label_inside_display = new System.Windows.Forms.Label();
			this.label_voltage_display = new System.Windows.Forms.Label();
			this.label_state_display = new System.Windows.Forms.Label();
			this.label_bonus_display = new System.Windows.Forms.Label();
			this.chartSummary = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.label1 = new System.Windows.Forms.Label();
			this.labelState = new System.Windows.Forms.Label();
			this.tabControl = new System.Windows.Forms.TabControl();
			this.tabConfig = new System.Windows.Forms.TabPage();
			this.button3 = new System.Windows.Forms.Button();
			this.button_cal = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.baudDropDown = new System.Windows.Forms.ComboBox();
			this.button1 = new System.Windows.Forms.Button();
			this.tabSummary = new System.Windows.Forms.TabPage();
			this.tabAccelerometer = new System.Windows.Forms.TabPage();
			this.chartAccel = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.tabAltitude = new System.Windows.Forms.TabPage();
			this.chartAltitude = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.tabVoltage = new System.Windows.Forms.TabPage();
			this.chartVoltage = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.tabOutsideTemp = new System.Windows.Forms.TabPage();
			this.chartOutsideTemp = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.tabInsideTemp = new System.Windows.Forms.TabPage();
			this.chartInsideTemp = new System.Windows.Forms.DataVisualization.Charting.Chart();
			this.tab_packets = new System.Windows.Forms.TabPage();
			this.richTextBox1 = new System.Windows.Forms.RichTextBox();
			this.clear_data_button = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.chartSummary)).BeginInit();
			this.tabControl.SuspendLayout();
			this.tabConfig.SuspendLayout();
			this.tabSummary.SuspendLayout();
			this.tabAccelerometer.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.chartAccel)).BeginInit();
			this.tabAltitude.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.chartAltitude)).BeginInit();
			this.tabVoltage.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.chartVoltage)).BeginInit();
			this.tabOutsideTemp.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.chartOutsideTemp)).BeginInit();
			this.tabInsideTemp.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.chartInsideTemp)).BeginInit();
			this.tab_packets.SuspendLayout();
			this.SuspendLayout();
			// 
			// serialPort1
			// 
			this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.SerialPort1DataReceived);
			// 
			// PortDropDown
			// 
			this.PortDropDown.FormattingEnabled = true;
			this.PortDropDown.Location = new System.Drawing.Point(112, 31);
			this.PortDropDown.Name = "PortDropDown";
			this.PortDropDown.Size = new System.Drawing.Size(121, 21);
			this.PortDropDown.TabIndex = 1;
			this.PortDropDown.SelectedIndexChanged += new System.EventHandler(this.PortDropDownSelectedIndexChanged);
			// 
			// label_port
			// 
			this.label_port.Location = new System.Drawing.Point(25, 31);
			this.label_port.Name = "label_port";
			this.label_port.Size = new System.Drawing.Size(81, 23);
			this.label_port.TabIndex = 2;
			this.label_port.Text = "Port to open:";
			// 
			// button_open
			// 
			this.button_open.Location = new System.Drawing.Point(112, 58);
			this.button_open.Name = "button_open";
			this.button_open.Size = new System.Drawing.Size(75, 23);
			this.button_open.TabIndex = 4;
			this.button_open.Text = "Open";
			this.button_open.UseVisualStyleBackColor = true;
			this.button_open.Click += new System.EventHandler(this.Button_openClick);
			// 
			// label_id_display
			// 
			this.label_id_display.Location = new System.Drawing.Point(162, 77);
			this.label_id_display.Name = "label_id_display";
			this.label_id_display.Size = new System.Drawing.Size(100, 23);
			this.label_id_display.TabIndex = 20;
			// 
			// label_time_display
			// 
			this.label_time_display.Location = new System.Drawing.Point(162, 110);
			this.label_time_display.Name = "label_time_display";
			this.label_time_display.Size = new System.Drawing.Size(100, 23);
			this.label_time_display.TabIndex = 21;
			// 
			// label_alt_display
			// 
			this.label_alt_display.Location = new System.Drawing.Point(162, 137);
			this.label_alt_display.Name = "label_alt_display";
			this.label_alt_display.Size = new System.Drawing.Size(100, 23);
			this.label_alt_display.TabIndex = 22;
			// 
			// label_outside_temp_display
			// 
			this.label_outside_temp_display.Location = new System.Drawing.Point(162, 164);
			this.label_outside_temp_display.Name = "label_outside_temp_display";
			this.label_outside_temp_display.Size = new System.Drawing.Size(100, 23);
			this.label_outside_temp_display.TabIndex = 23;
			// 
			// label_inside_display
			// 
			this.label_inside_display.Location = new System.Drawing.Point(162, 197);
			this.label_inside_display.Name = "label_inside_display";
			this.label_inside_display.Size = new System.Drawing.Size(100, 23);
			this.label_inside_display.TabIndex = 24;
			// 
			// label_voltage_display
			// 
			this.label_voltage_display.Location = new System.Drawing.Point(162, 234);
			this.label_voltage_display.Name = "label_voltage_display";
			this.label_voltage_display.Size = new System.Drawing.Size(100, 23);
			this.label_voltage_display.TabIndex = 25;
			// 
			// label_state_display
			// 
			this.label_state_display.Location = new System.Drawing.Point(162, 261);
			this.label_state_display.Name = "label_state_display";
			this.label_state_display.Size = new System.Drawing.Size(100, 23);
			this.label_state_display.TabIndex = 26;
			// 
			// label_bonus_display
			// 
			this.label_bonus_display.Location = new System.Drawing.Point(162, 288);
			this.label_bonus_display.Name = "label_bonus_display";
			this.label_bonus_display.Size = new System.Drawing.Size(100, 23);
			this.label_bonus_display.TabIndex = 27;
			// 
			// chartSummary
			// 
			chartArea1.AxisX.Title = "Time";
			chartArea1.AxisY.Title = "Altitude (m)";
			chartArea1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea1.Name = "ChartArea1";
			chartArea1.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea2.AxisX.Title = "Time";
			chartArea2.AxisY.Title = "Voltage (V)";
			chartArea2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea2.Name = "ChartArea2";
			chartArea3.AxisX.Title = "Time";
			chartArea3.AxisY.Title = "Outside Temp (deg C)";
			chartArea3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea3.Name = "ChartArea3";
			chartArea4.AxisX.Title = "Time";
			chartArea4.AxisY.Title = "Inside Temp (deg C)";
			chartArea4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea4.Name = "ChartArea4";
			this.chartSummary.ChartAreas.Add(chartArea1);
			this.chartSummary.ChartAreas.Add(chartArea2);
			this.chartSummary.ChartAreas.Add(chartArea3);
			this.chartSummary.ChartAreas.Add(chartArea4);
			this.chartSummary.Location = new System.Drawing.Point(0, 0);
			this.chartSummary.Name = "chartSummary";
			this.chartSummary.Palette = System.Windows.Forms.DataVisualization.Charting.ChartColorPalette.Bright;
			series1.BorderWidth = 4;
			series1.ChartArea = "ChartArea1";
			series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series1.Color = System.Drawing.Color.Yellow;
			series1.MarkerColor = System.Drawing.Color.Transparent;
			series1.Name = "Altitude";
			series1.ShadowColor = System.Drawing.Color.Gray;
			series2.BorderWidth = 4;
			series2.ChartArea = "ChartArea2";
			series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series2.Color = System.Drawing.Color.Yellow;
			series2.Name = "Voltage";
			series3.BorderWidth = 4;
			series3.ChartArea = "ChartArea3";
			series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series3.Color = System.Drawing.Color.Yellow;
			series3.Name = "Outside Temperature";
			series4.BorderWidth = 4;
			series4.ChartArea = "ChartArea4";
			series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series4.Color = System.Drawing.Color.Yellow;
			series4.Name = "Inside Temperature";
			this.chartSummary.Series.Add(series1);
			this.chartSummary.Series.Add(series2);
			this.chartSummary.Series.Add(series3);
			this.chartSummary.Series.Add(series4);
			this.chartSummary.Size = new System.Drawing.Size(1110, 580);
			this.chartSummary.TabIndex = 28;
			this.chartSummary.Text = "SummaryChart";
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Calibri", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(462, 12);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(117, 23);
			this.label1.TabIndex = 29;
			this.label1.Text = "Flight State:";
			// 
			// labelState
			// 
			this.labelState.Font = new System.Drawing.Font("Calibri", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelState.Location = new System.Drawing.Point(585, 11);
			this.labelState.Name = "labelState";
			this.labelState.Size = new System.Drawing.Size(250, 23);
			this.labelState.TabIndex = 30;
			this.labelState.Text = "None";
			// 
			// tabControl
			// 
			this.tabControl.Controls.Add(this.tabConfig);
			this.tabControl.Controls.Add(this.tabSummary);
			this.tabControl.Controls.Add(this.tabAccelerometer);
			this.tabControl.Controls.Add(this.tabAltitude);
			this.tabControl.Controls.Add(this.tabVoltage);
			this.tabControl.Controls.Add(this.tabOutsideTemp);
			this.tabControl.Controls.Add(this.tabInsideTemp);
			this.tabControl.Controls.Add(this.tab_packets);
			this.tabControl.Location = new System.Drawing.Point(29, 39);
			this.tabControl.Name = "tabControl";
			this.tabControl.SelectedIndex = 0;
			this.tabControl.Size = new System.Drawing.Size(1130, 600);
			this.tabControl.TabIndex = 31;
			// 
			// tabConfig
			// 
			this.tabConfig.Controls.Add(this.button3);
			this.tabConfig.Controls.Add(this.button_cal);
			this.tabConfig.Controls.Add(this.button2);
			this.tabConfig.Controls.Add(this.label2);
			this.tabConfig.Controls.Add(this.baudDropDown);
			this.tabConfig.Controls.Add(this.button1);
			this.tabConfig.Controls.Add(this.PortDropDown);
			this.tabConfig.Controls.Add(this.label_port);
			this.tabConfig.Controls.Add(this.button_open);
			this.tabConfig.Location = new System.Drawing.Point(4, 22);
			this.tabConfig.Name = "tabConfig";
			this.tabConfig.Padding = new System.Windows.Forms.Padding(3);
			this.tabConfig.Size = new System.Drawing.Size(1122, 574);
			this.tabConfig.TabIndex = 6;
			this.tabConfig.Text = "Options";
			this.tabConfig.UseVisualStyleBackColor = true;
			this.tabConfig.Click += new System.EventHandler(this.TabConfigClick);
			// 
			// button3
			// 
			this.button3.Location = new System.Drawing.Point(25, 316);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(181, 23);
			this.button3.TabIndex = 37;
			this.button3.Text = "Return to Regular Transmission";
			this.button3.UseVisualStyleBackColor = true;
			this.button3.Click += new System.EventHandler(this.Button3Click);
			// 
			// button_cal
			// 
			this.button_cal.Location = new System.Drawing.Point(25, 381);
			this.button_cal.Name = "button_cal";
			this.button_cal.Size = new System.Drawing.Size(181, 23);
			this.button_cal.TabIndex = 36;
			this.button_cal.Text = "Initiate Calibration";
			this.button_cal.UseVisualStyleBackColor = true;
			this.button_cal.Click += new System.EventHandler(this.Button_calClick);
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(25, 286);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(181, 23);
			this.button2.TabIndex = 35;
			this.button2.Text = "Dump EEPROM Data";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.Button2Click);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(25, 173);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(81, 23);
			this.label2.TabIndex = 34;
			this.label2.Text = "Baud Rate:";
			// 
			// baudDropDown
			// 
			this.baudDropDown.FormattingEnabled = true;
			this.baudDropDown.Items.AddRange(new object[] {
			"1200",
			"2400",
			"4800",
			"9600",
			"19200",
			"38400",
			"57600",
			"115200"});
			this.baudDropDown.Location = new System.Drawing.Point(112, 173);
			this.baudDropDown.Name = "baudDropDown";
			this.baudDropDown.Size = new System.Drawing.Size(121, 21);
			this.baudDropDown.TabIndex = 33;
			this.baudDropDown.SelectedIndexChanged += new System.EventHandler(this.baudDropDownSelectedIndexChanged);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(253, 29);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(89, 23);
			this.button1.TabIndex = 32;
			this.button1.Text = "Refresh Ports";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.Button1Click);
			// 
			// tabSummary
			// 
			this.tabSummary.Controls.Add(this.chartSummary);
			this.tabSummary.Location = new System.Drawing.Point(4, 22);
			this.tabSummary.Name = "tabSummary";
			this.tabSummary.Padding = new System.Windows.Forms.Padding(3);
			this.tabSummary.Size = new System.Drawing.Size(1122, 574);
			this.tabSummary.TabIndex = 0;
			this.tabSummary.Text = "Summary";
			this.tabSummary.UseVisualStyleBackColor = true;
			// 
			// tabAccelerometer
			// 
			this.tabAccelerometer.Controls.Add(this.chartAccel);
			this.tabAccelerometer.Location = new System.Drawing.Point(4, 22);
			this.tabAccelerometer.Name = "tabAccelerometer";
			this.tabAccelerometer.Padding = new System.Windows.Forms.Padding(3);
			this.tabAccelerometer.Size = new System.Drawing.Size(1122, 574);
			this.tabAccelerometer.TabIndex = 5;
			this.tabAccelerometer.Text = "Accelerometer";
			this.tabAccelerometer.UseVisualStyleBackColor = true;
			// 
			// chartAccel
			// 
			chartArea5.AxisY.Title = "X";
			chartArea5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea5.Name = "ChartArea1";
			chartArea6.AxisY.Title = " Y";
			chartArea6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea6.Name = "ChartArea2";
			chartArea7.AxisX.Title = "Time (s)";
			chartArea7.AxisY.Title = "Z";
			chartArea7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea7.Name = "ChartArea3";
			this.chartAccel.ChartAreas.Add(chartArea5);
			this.chartAccel.ChartAreas.Add(chartArea6);
			this.chartAccel.ChartAreas.Add(chartArea7);
			this.chartAccel.Location = new System.Drawing.Point(0, 0);
			this.chartAccel.Name = "chartAccel";
			series5.BorderWidth = 4;
			series5.ChartArea = "ChartArea1";
			series5.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series5.Color = System.Drawing.Color.Yellow;
			series5.MarkerColor = System.Drawing.Color.Transparent;
			series5.Name = "X";
			series5.YValuesPerPoint = 2;
			series6.BorderWidth = 4;
			series6.ChartArea = "ChartArea2";
			series6.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series6.Color = System.Drawing.Color.Yellow;
			series6.Name = "Y";
			series7.BorderWidth = 4;
			series7.ChartArea = "ChartArea3";
			series7.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series7.Color = System.Drawing.Color.Yellow;
			series7.Name = "Z";
			this.chartAccel.Series.Add(series5);
			this.chartAccel.Series.Add(series6);
			this.chartAccel.Series.Add(series7);
			this.chartAccel.Size = new System.Drawing.Size(1110, 580);
			this.chartAccel.TabIndex = 0;
			this.chartAccel.Text = "chartAccel";
			// 
			// tabAltitude
			// 
			this.tabAltitude.Controls.Add(this.chartAltitude);
			this.tabAltitude.Location = new System.Drawing.Point(4, 22);
			this.tabAltitude.Name = "tabAltitude";
			this.tabAltitude.Padding = new System.Windows.Forms.Padding(3);
			this.tabAltitude.Size = new System.Drawing.Size(1122, 574);
			this.tabAltitude.TabIndex = 1;
			this.tabAltitude.Text = "Altitude";
			this.tabAltitude.UseVisualStyleBackColor = true;
			// 
			// chartAltitude
			// 
			chartArea8.AxisX.Title = "Time (s)";
			chartArea8.AxisY.Title = "Altitude(m)";
			chartArea8.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea8.Name = "ChartArea1";
			this.chartAltitude.ChartAreas.Add(chartArea8);
			this.chartAltitude.Location = new System.Drawing.Point(0, 0);
			this.chartAltitude.Name = "chartAltitude";
			series8.BorderWidth = 4;
			series8.ChartArea = "ChartArea1";
			series8.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series8.Color = System.Drawing.Color.Yellow;
			series8.Name = "Altitude";
			this.chartAltitude.Series.Add(series8);
			this.chartAltitude.Size = new System.Drawing.Size(1110, 580);
			this.chartAltitude.TabIndex = 0;
			this.chartAltitude.Text = "chart2";
			title1.Name = "Title1";
			this.chartAltitude.Titles.Add(title1);
			// 
			// tabVoltage
			// 
			this.tabVoltage.Controls.Add(this.chartVoltage);
			this.tabVoltage.Location = new System.Drawing.Point(4, 22);
			this.tabVoltage.Name = "tabVoltage";
			this.tabVoltage.Padding = new System.Windows.Forms.Padding(3);
			this.tabVoltage.Size = new System.Drawing.Size(1122, 574);
			this.tabVoltage.TabIndex = 2;
			this.tabVoltage.Text = "Voltage";
			this.tabVoltage.UseVisualStyleBackColor = true;
			// 
			// chartVoltage
			// 
			chartArea9.AxisX.Title = "Time (s)";
			chartArea9.AxisY.Title = "Voltage (V)";
			chartArea9.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea9.Name = "ChartArea1";
			this.chartVoltage.ChartAreas.Add(chartArea9);
			this.chartVoltage.Location = new System.Drawing.Point(15, 9);
			this.chartVoltage.Name = "chartVoltage";
			series9.BorderWidth = 4;
			series9.ChartArea = "ChartArea1";
			series9.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series9.Color = System.Drawing.Color.Yellow;
			series9.Name = "Voltage";
			this.chartVoltage.Series.Add(series9);
			this.chartVoltage.Size = new System.Drawing.Size(1110, 560);
			this.chartVoltage.TabIndex = 0;
			this.chartVoltage.Text = "chart3";
			// 
			// tabOutsideTemp
			// 
			this.tabOutsideTemp.Controls.Add(this.chartOutsideTemp);
			this.tabOutsideTemp.Location = new System.Drawing.Point(4, 22);
			this.tabOutsideTemp.Name = "tabOutsideTemp";
			this.tabOutsideTemp.Padding = new System.Windows.Forms.Padding(3);
			this.tabOutsideTemp.Size = new System.Drawing.Size(1122, 574);
			this.tabOutsideTemp.TabIndex = 3;
			this.tabOutsideTemp.Text = "Outside Temperature";
			this.tabOutsideTemp.UseVisualStyleBackColor = true;
			// 
			// chartOutsideTemp
			// 
			chartArea10.AxisX.Title = "Time (s)";
			chartArea10.AxisY.Title = "Outside Temperature (deg C)";
			chartArea10.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea10.Name = "ChartArea1";
			this.chartOutsideTemp.ChartAreas.Add(chartArea10);
			this.chartOutsideTemp.Location = new System.Drawing.Point(10, 10);
			this.chartOutsideTemp.Name = "chartOutsideTemp";
			series10.BorderWidth = 4;
			series10.ChartArea = "ChartArea1";
			series10.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series10.Color = System.Drawing.Color.Yellow;
			series10.Name = "Outside Temperature";
			this.chartOutsideTemp.Series.Add(series10);
			this.chartOutsideTemp.Size = new System.Drawing.Size(1110, 560);
			this.chartOutsideTemp.TabIndex = 0;
			this.chartOutsideTemp.Text = "chart4";
			// 
			// tabInsideTemp
			// 
			this.tabInsideTemp.Controls.Add(this.chartInsideTemp);
			this.tabInsideTemp.Location = new System.Drawing.Point(4, 22);
			this.tabInsideTemp.Name = "tabInsideTemp";
			this.tabInsideTemp.Padding = new System.Windows.Forms.Padding(3);
			this.tabInsideTemp.Size = new System.Drawing.Size(1122, 574);
			this.tabInsideTemp.TabIndex = 4;
			this.tabInsideTemp.Text = "Inside Temperature";
			this.tabInsideTemp.UseVisualStyleBackColor = true;
			// 
			// chartInsideTemp
			// 
			chartArea11.AxisX.Title = "Time (s)";
			chartArea11.AxisY.Title = "Inside Temperature (deg C)";
			chartArea11.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			chartArea11.Name = "ChartArea1";
			this.chartInsideTemp.ChartAreas.Add(chartArea11);
			this.chartInsideTemp.Location = new System.Drawing.Point(10, 10);
			this.chartInsideTemp.Name = "chartInsideTemp";
			series11.BorderWidth = 4;
			series11.ChartArea = "ChartArea1";
			series11.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
			series11.Color = System.Drawing.Color.Yellow;
			series11.Name = "Inside Temperature";
			this.chartInsideTemp.Series.Add(series11);
			this.chartInsideTemp.Size = new System.Drawing.Size(1110, 560);
			this.chartInsideTemp.TabIndex = 0;
			this.chartInsideTemp.Text = "chart5";
			// 
			// tab_packets
			// 
			this.tab_packets.Controls.Add(this.clear_data_button);
			this.tab_packets.Controls.Add(this.richTextBox1);
			this.tab_packets.Location = new System.Drawing.Point(4, 22);
			this.tab_packets.Name = "tab_packets";
			this.tab_packets.Padding = new System.Windows.Forms.Padding(3);
			this.tab_packets.Size = new System.Drawing.Size(1122, 574);
			this.tab_packets.TabIndex = 7;
			this.tab_packets.Text = "Raw Packets";
			this.tab_packets.UseVisualStyleBackColor = true;
			// 
			// richTextBox1
			// 
			this.richTextBox1.Location = new System.Drawing.Point(55, 29);
			this.richTextBox1.Name = "richTextBox1";
			this.richTextBox1.Size = new System.Drawing.Size(1010, 500);
			this.richTextBox1.TabIndex = 0;
			this.richTextBox1.Text = "";
			// 
			// clear_data_button
			// 
			this.clear_data_button.Location = new System.Drawing.Point(941, 535);
			this.clear_data_button.Name = "clear_data_button";
			this.clear_data_button.Size = new System.Drawing.Size(75, 23);
			this.clear_data_button.TabIndex = 1;
			this.clear_data_button.Text = "Clear Data";
			this.clear_data_button.UseVisualStyleBackColor = true;
			this.clear_data_button.Click += new System.EventHandler(this.Clear_data_buttonClick);
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoValidate = System.Windows.Forms.AutoValidate.Disable;
			this.ClientSize = new System.Drawing.Size(1184, 661);
			this.Controls.Add(this.tabControl);
			this.Controls.Add(this.labelState);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.label_bonus_display);
			this.Controls.Add(this.label_state_display);
			this.Controls.Add(this.label_voltage_display);
			this.Controls.Add(this.label_inside_display);
			this.Controls.Add(this.label_outside_temp_display);
			this.Controls.Add(this.label_alt_display);
			this.Controls.Add(this.label_time_display);
			this.Controls.Add(this.label_id_display);
			this.Name = "MainForm";
			this.Text = "cansat";
			this.Load += new System.EventHandler(this.MainFormLoad);
			((System.ComponentModel.ISupportInitialize)(this.chartSummary)).EndInit();
			this.tabControl.ResumeLayout(false);
			this.tabConfig.ResumeLayout(false);
			this.tabSummary.ResumeLayout(false);
			this.tabAccelerometer.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.chartAccel)).EndInit();
			this.tabAltitude.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.chartAltitude)).EndInit();
			this.tabVoltage.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.chartVoltage)).EndInit();
			this.tabOutsideTemp.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.chartOutsideTemp)).EndInit();
			this.tabInsideTemp.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.chartInsideTemp)).EndInit();
			this.tab_packets.ResumeLayout(false);
			this.ResumeLayout(false);

		}
	}
}
