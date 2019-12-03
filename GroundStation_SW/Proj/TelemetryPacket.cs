/*
 * Created by SharpDevelop.
 * User: Graham
 * Date: 03/10/2014
 * Time: 11:09 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;

namespace cansat
{
	/// <summary>
	/// Description of TelemetryPacket.
	/// </summary>
	public class TelemetryPacket
	{
		public int team_id;
		public int time;
		public short altitude;
		public double outside_temp;
		public double inside_temp;
		public float voltage;
		public string state;
		public float x_accel;
		public float y_accel;
		public float z_accel;
		public int count;
			
	
		public TelemetryPacket(){
			
		}
		
		public string stateName(int i){
			
			//int i = Convert.ToInt32(num);
			
			switch(i)
			{
				case 0:
					return "Test";
				case 1:
					return "Launch";
				case 2:
					return "Ascent";
				case 3:
					return "Pre-deployment of payload";
				case 4:
					return "Payload descent";
				case 5:
					return "Land";
				default:
					return "Error";
			}
		
		}
	}
}
