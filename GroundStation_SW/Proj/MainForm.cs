/*
 * Created by SharpDevelop.
 * User: Graham
 * Date: 27/
* 09/2014
 * Time: 5:03 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Text;

namespace cansat
{
	/// <summary>
	/// Description of MainForm.
	/// </summary>
	public partial class MainForm : Form
	{
		const int packetSize = 37;
		bool transmit = true;
		const float maxVoltage = 2.5F;
		const float voltageScale = 1.8466F;
		string lastState = "Test";
		string folder;
			
		public MainForm()
		{
			
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			
			InitializeComponent();
			
			string currentDir = Directory.GetCurrentDirectory();
			string timestamp = "CanSat " + string.Format("{0:yyyy-MM-dd_hh-mm-ss-tt}", DateTime.Now);
			string folder = Path.Combine(currentDir, timestamp);
			Directory.CreateDirectory(folder);
			
			HandleData.initializeCSV(folder);
			PortDropDown.Items.AddRange(SerialPort.GetPortNames());

			//
			// TODO: Add constructor code after the InitializeComponent() call.
			//
		}
		void PortDropDownSelectedIndexChanged(object sender, EventArgs e)
		{
			serialPort1.PortName = PortDropDown.SelectedItem.ToString();
		}
		void Button_openClick(object sender, EventArgs e)
		{
			if(button_open.Text == "Open"){

				try{
					serialPort1.Open();
					button_open.Text = "Close";	
				} catch (Exception ex){
					MessageBox.Show(ex.Message);
				}
				
			}
			else{
				serialPort1.Close();
				button_open.Text = "Open";
			}
		}
			
		void SerialPort1DataReceived(object sender, SerialDataReceivedEventArgs e){
			if(transmit){
			readTransmittedTelemetry();
			} else {
				readEEPROM();
			}
		}
		
		void readTransmittedTelemetry(){
			if(serialPort1.BytesToRead <= packetSize){
				return;
			}
			//StreamWriter rawDataFile = new StreamWriter("raw_packets.txt");
			byte[] firstByte = new byte[4];
			serialPort1.Read(firstByte, 0, 4);
			//rawDataFile.Write(firstByte);
			if(firstByte[0] != 0x7E || firstByte[1] != 0x00 || firstByte[2] != 0x21 || firstByte[3] != 0x90){
				return;
			}
			byte[] rawPacket = new byte[packetSize-4];
			serialPort1.Read(rawPacket, 0, packetSize-4);
			//rawDataFile.Write(rawPacket);
			//rawDataFile.Close();
			var ms = new MemoryStream(new byte[packetSize], 0, packetSize, true, true);
			ms.Write(firstByte, 0, 4);
			ms.Write(rawPacket, 0, packetSize-4);
			byte[] fullPacket = ms.GetBuffer();//maybe use ToArray??
			
			string packetStr = BitConverter.ToString(fullPacket, 0, packetSize);
			richTextBox1.Invoke(new MethodInvoker(delegate() {richTextBox1.AppendText(packetStr + Environment.NewLine);}));
			richTextBox1.Invoke(new MethodInvoker(delegate() {richTextBox1.ScrollToCaret();}));
			
			
			TelemetryPacket receivedPacket = new TelemetryPacket();
			parsePacket(ref receivedPacket, fullPacket);
			if(receivedPacket.time < 0){
				return;
			}
			//reset graphs when leaving lest state;
			if(lastState.Equals("Test") && receivedPacket.state.Equals("Lauch")){
				HandleData.clearData();
				updateCharts();
			}
			
			HandleData.addPacketToCSV(HandleData.packetToString(receivedPacket)); // add ToString method to TelemetryPacket
			HandleData.addNewPacket(receivedPacket);
			updateCharts();
			lastState = receivedPacket.state;
		}
		
		void parsePacket(ref TelemetryPacket packet, byte[] fullData){
			
			packet.team_id = ((int)fullData[15] << 8) + (int)fullData[16];
			packet.time = ((int)fullData[17] << 8) + (int)fullData[18];
			packet.altitude = unchecked((short)(((ushort)fullData[19] << 8) + fullData[20]));
			packet.outside_temp =(((int)fullData[21] << 8) + (int)fullData[22])/10;
			packet.inside_temp = (((int)fullData[23] << 8) + (int)fullData[24])/10;

			packet.voltage = unchecked((ushort)(((ushort)fullData[25] << 8) + fullData[26]));
			packet.voltage = unchecked((float)(((packet.voltage)/65536)*(maxVoltage*voltageScale)));
			packet.state = packet.stateName((int)fullData[27]);
			packet.x_accel = unchecked((short)(((ushort)fullData[28] << 8) + fullData[29]));
			packet.x_accel = unchecked((float)(packet.x_accel/16384));
			packet.y_accel = unchecked((short)(((ushort)fullData[30] << 8) + fullData[31]));
			packet.y_accel = unchecked((float)(packet.y_accel/16384));
			packet.z_accel = unchecked((short)(((ushort)fullData[32] << 8) + fullData[33]));
			packet.z_accel = unchecked((float)(packet.z_accel/16384));
			packet.count = ((int)fullData[34] << 8) + (int)fullData[35];

		}

		void readEEPROM(){
			if(serialPort1.BytesToRead < 256){
				return;
			}
			StreamWriter CSV = new StreamWriter("EEPROM_DATA.CSV", true);
			
			
			byte[] newBytes = new byte[256];
			serialPort1.Read(newBytes, 0, 256);
			string Str = BitConverter.ToString(newBytes, 0, 256);
			richTextBox1.Invoke(new MethodInvoker(delegate() {richTextBox1.AppendText(Str + Environment.NewLine + Environment.NewLine);}));
			TelemetryPacket packet = new TelemetryPacket();
			for(int i = 0; i <=12; i++){
				if(newBytes[21*i] == 0x03 && newBytes[21*i + 1] == 0xE9){
					packet.team_id = 1001;
				
					packet.time = ((int)newBytes[21*i + 2] << 8) + (int)newBytes[21*i + 3];
				
					packet.altitude = unchecked((short)(((ushort)newBytes[21*i + 4] << 8) + newBytes[21*i + 5]));
				
					packet.outside_temp =(((int)newBytes[21*i + 6] << 8) + (int)newBytes[21*i + 7])/10;
				
					packet.inside_temp = (((int)newBytes[21*i + 8] << 8) + (int)newBytes[21*i + 9])/10;
				
					packet.voltage = unchecked((ushort)(((ushort)newBytes[21*i + 10] << 8) + newBytes[21*i +11]));
					packet.voltage = unchecked((float)(((packet.voltage)/65535)*(maxVoltage*voltageScale)));
				
					packet.state = packet.stateName((int)newBytes[21*i + 12]);
				
					packet.x_accel = unchecked((short)(((ushort)newBytes[21*i + 13] << 8) + newBytes[21*i + 14]));
					packet.x_accel = unchecked((float)(packet.x_accel/16384));
				
					packet.y_accel = unchecked((short)(((ushort)newBytes[21*i + 15] << 8) + newBytes[21*i + 16]));
					packet.y_accel = unchecked((float)(packet.y_accel/16384));
				
					packet.z_accel = unchecked((short)(((ushort)newBytes[21*i + 17] << 8) + newBytes[21*i + 18]));
					packet.z_accel = unchecked((float)(packet.z_accel/16384));
				
					packet.count = ((int)newBytes[21*i + 19] << 8) + (int)newBytes[21*i + 20];
				
					string[] packetStr = HandleData.packetToString(packet);
				
					string row = String.Format("{0, 8}, {1, 8}, {2, 8}, {3, 14}, {4, 14}, {5, 8}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}", packetStr[0], packetStr[1], packetStr[2], packetStr[3],
				                           packetStr[4], packetStr[5], packetStr[6], packetStr[7], packetStr[8], packetStr[9], packetStr[10]);
					CSV.WriteLine(row);
				}
			}
			CSV.Close();
			//MessageBox.Show("Done");
		}
		
		public void updateCharts(){
			
			chartSummary.Invoke(
				new MethodInvoker(
					delegate() {chartSummary.Series["Altitude"].Points.DataBindXY(HandleData.getTime(), HandleData.getAltitude());}));
			
			chartSummary.Invoke(
				new MethodInvoker(
					delegate() {chartSummary.Series["Outside Temperature"].Points.DataBindXY(HandleData.getTime(), HandleData.getOutsideTemp());}));
			
			chartSummary.Invoke(
				new MethodInvoker(
					delegate() {chartSummary.Series["Inside Temperature"].Points.DataBindXY(HandleData.getTime(), HandleData.getInsideTemp());}));
			
			chartSummary.Invoke(
				new MethodInvoker(
					delegate() {chartSummary.Series["Voltage"].Points.DataBindXY(HandleData.getTime(), HandleData.getVoltage());}));
//Individual tabs

			chartAltitude.Invoke(
				new MethodInvoker(
					delegate() {chartAltitude.Series["Altitude"].Points.DataBindXY(HandleData.getTime(), HandleData.getAltitude());}));
			
			chartOutsideTemp.Invoke(
				new MethodInvoker(
					delegate() {chartOutsideTemp.Series["Outside Temperature"].Points.DataBindXY(HandleData.getTime(), HandleData.getOutsideTemp());}));
			
			chartInsideTemp.Invoke(
				new MethodInvoker(
					delegate() {chartInsideTemp.Series["Inside Temperature"].Points.DataBindXY(HandleData.getTime(), HandleData.getInsideTemp());}));
		
			chartVoltage.Invoke(
				new MethodInvoker(
					delegate() {chartVoltage.Series["Voltage"].Points.DataBindXY(HandleData.getTime(), HandleData.getVoltage());}));

			chartAccel.Invoke(
				new MethodInvoker(
					delegate() {chartAccel.Series["X"].Points.DataBindXY(HandleData.getTime(), HandleData.getXaccel());}));

			chartAccel.Invoke(
				new MethodInvoker(
					delegate() {chartAccel.Series["Y"].Points.DataBindXY(HandleData.getTime(), HandleData.getYaccel());}));
					
			chartAccel.Invoke(
				new MethodInvoker(
					delegate() {chartAccel.Series["Z"].Points.DataBindXY(HandleData.getTime(), HandleData.getZaccel());}));
			
			labelState.Invoke(new MethodInvoker(delegate() {labelState.Text = HandleData.getState();}));
		
		}
			
		public void Button1Click(object sender, EventArgs e)
		{
			PortDropDown.Items.Clear();
			PortDropDown.Items.AddRange(SerialPort.GetPortNames());
		}
		
		void baudDropDownSelectedIndexChanged(object sender, EventArgs e)
		{
			//MessageBox.Show(baudDropDown.Text);
			serialPort1.BaudRate = Convert.ToInt32(baudDropDown.Text);
			//serialPort1.Invoke(new MethodInvoker(delegate() {serialPort1.BaudRate = Convert.ToInt32(baudDropDown.SelectedText);}));
		}
		
		void TabConfigClick(object sender, EventArgs e)
		{
	
		}
		
		//dump eeprom
		void Button2Click(object sender, EventArgs e)
		{
			try{
			transmit = false;
			string file = Path.Combine(folder, "EEPROM_DATA.CSV");
			StreamWriter CSV = new StreamWriter(file, true);
			
			string titles = String.Format("{0, 8}, {1, 8}, {2, 8}, {3, 14}, {4, 14}, {5, 8}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6} ", "Team ID", "Time", "Altitude", "Outside Temp",
			                           "Inside Temp", "Voltage", "State", "Xaccel", "Yaccel", "Zaccel", "Count");
			CSV.WriteLine(titles);
			CSV.Close();
			byte[] sendByte = new byte[1];
			sendByte[0] = 0xFE;
			serialPort1.Write(sendByte, 0, 1);
			}catch(Exception ex){
				MessageBox.Show(ex.Message);
			}
		}
		void MainFormLoad(object sender, EventArgs e)
		{
	
		}
		void Button_calClick(object sender, EventArgs e)
		{
			try{
				char[] toSend = new char[1];
				toSend[0] = 'c';
				serialPort1.Write(toSend, 0, 1);
			}catch(Exception ex){
				MessageBox.Show(ex.Message);
			}
		}
		void Button3Click(object sender, EventArgs e)
		{
			transmit = true;
		}
		void Clear_data_buttonClick(object sender, EventArgs e)
		{
			richTextBox1.ResetText();
		}
		
	}
	

}
