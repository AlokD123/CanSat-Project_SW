/*
 * Created by SharpDevelop.
 * User: Graham
 * Date: 03/10/2014
 * Time: 10:05 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.IO;
using System.Collections.Generic;

namespace cansat
{
	/// <summary>
	/// Description of Class1.
	/// </summary>
	public static class HandleData
	{

		public const string fileName = "CANSAT2015_TLM_1001.csv";
		static string file;
		private static List<int> time = new List<int>();
		private static List<double> altitude = new List<double>();
		private static List<double> outside_temp = new List<double>();
		private static List<double> inside_temp = new List<double>();
		private static List<double> voltage = new List<double>();
		private static List<double> x_accel = new List<double>();
		private static List<double> y_accel = new List<double>();
		private static List<double> z_accel = new List<double>();
		private static string state;
		
		public static void initializeCSV(string folder){
			file = Path.Combine(folder, fileName);
			
			StreamWriter CSV = new StreamWriter(file, true);
			
			string titles = String.Format("{0, 8}, {1, 8}, {2, 8}, {3, 14}, {4, 14}, {5, 8}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6} ", "Team ID", "Time", "Altitude", "Outside Temp",
			                           "Inside Temp", "Voltage", "State", "Xaccel", "Yaccel", "Zaccel", "Count");
			CSV.WriteLine(titles);
			CSV.Close();
		}
		
		public static void addNewPacket(TelemetryPacket telemetry){
			time.Add(telemetry.time);
			altitude.Add(telemetry.altitude);
			outside_temp.Add(telemetry.outside_temp);
			inside_temp.Add(telemetry.inside_temp);
			voltage.Add(telemetry.voltage);
			x_accel.Add(telemetry.x_accel);
			y_accel.Add(telemetry.y_accel);
			z_accel.Add(telemetry.z_accel);
			state = telemetry.state;
		}
		
		public static void addPacketToCSV(string[] packet){
			StreamWriter CSV = new StreamWriter(file, true);

			string row = String.Format("{0, 8}, {1, 8}, {2, 8}, {3, 14}, {4, 14}, {5, 8}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}", packet[0], packet[1], packet[2], packet[3],
			                           packet[4], packet[5], packet[6], packet[7], packet[8], packet[9], packet[10]);
			CSV.WriteLine(row);
			CSV.Close();
			
		}
		
		public static string[] packetToString(TelemetryPacket data){
			string[] packetStr = new string[11];
			
			packetStr[0] = data.team_id.ToString();
			packetStr[1] = data.time.ToString();
			packetStr[2] = data.altitude.ToString();
			packetStr[3] = data.outside_temp.ToString();
			packetStr[4] = data.inside_temp.ToString();
			packetStr[5] = data.voltage.ToString();
			packetStr[6] = data.state;
			packetStr[7] = data.x_accel.ToString();
			packetStr[8] = data.y_accel.ToString();
			packetStr[9] = data.z_accel.ToString();
			packetStr[10] = data.count.ToString();
			return packetStr;
		}
		
		public static List<int> getTime(){
			return time;
		}
		
		public static List<double> getAltitude(){
			return altitude;
		}
		
		public static List<double> getOutsideTemp(){
			return outside_temp;
		}
		
		public static List<double> getInsideTemp(){
			return inside_temp;
		}
		
		public static List<double> getVoltage(){
			return voltage;
		}
		
		public static List<double> getXaccel(){
			return x_accel;
		}
		
		public static List<double> getYaccel(){
			return y_accel;
		}
		
		public static List<double> getZaccel(){
			return z_accel;
		}
		
		public static string getState(){
			return state;
		}
		
		public static void clearData(){
			time.Clear();
			altitude.Clear();
			outside_temp.Clear();
			inside_temp.Clear();
			voltage.Clear();
			x_accel.Clear();
			y_accel.Clear();
			z_accel.Clear();
		}
	}
}
