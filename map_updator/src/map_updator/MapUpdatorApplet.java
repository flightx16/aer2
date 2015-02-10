package map_updator;
import java.applet.*; 
import java.awt.*; 
import java.awt.event.KeyEvent;

import static java.awt.event.KeyEvent.*;

import java.awt.event.KeyListener;
import java.io.BufferedReader;
import java.io.OutputStream;
import java.net.URL;
import java.util.Enumeration;

import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 
import java.io.InputStreamReader;
import java.io.OutputStream;
import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 
import java.util.Enumeration;

public class MapUpdatorApplet extends Applet
		implements KeyListener, SerialPortEventListener {
	
   Image img;
   MediaTracker tr;
   URL imageURL;
   
   boolean pointsVertical;
   boolean pointsPositive;
   int x;
   int y;
   
   final int top = 24;
   final int left = 71;
   final int right = 464;
   final int bottom = 466;
   // fobiddenPoints come in pairs!!! x comes first, y comes second
   final int[] fobiddenPoints = {0, 9, 8, 9,4,4};
   
   SerialPort serialPort;
   
   public void init() {
	   this.x = 5;
	   this.y = 6;
	   this.pointsVertical = false;
	   this.pointsPositive = true;
	   this.setSize(554, 518);
	   this.addKeyListener(this);
	   this.initialize();
   }
   
   public void paint(Graphics g) {
      tr = new MediaTracker(this);
      imageURL = this.getClass().getResource("map.jpg");
      img = getImage(imageURL);
      tr.addImage(img,0);
      g.drawImage(img, 0, 0, this);
      g.setColor(Color.RED);
      int pX = x * (right - left) / 8 + left;
      int pY = y * (bottom - top) / 9 + top;
      if (pointsVertical)
    	  g.fillPolygon(new int[]{pX - 5, pX, pX + 5}, 
        		  new int[]{pY, pY + (pointsPositive ? 15 : -15), pY}, 3);
      else
    	  g.fillPolygon(new int[]{pX, pX + (pointsPositive ? 15 : -15), pX}, 
    		  new int[]{pY + 5, pY, pY - 5}, 3);
   }

@Override
public void keyPressed(KeyEvent event) {
	int x = this.x;
	int y = this.y;
	boolean vertical = this.pointsVertical;
	boolean positive = this.pointsPositive;
	switch (event.getKeyCode()) {
	case VK_UP: y --; vertical = true; positive = false; break;
	case VK_DOWN: y ++; vertical = true; positive = true; break;
	case VK_LEFT: x --; vertical = false; positive = false; break;
	case VK_RIGHT: x ++; vertical = false; positive = true; break;
	}
	
	if (x < 0 || x > 8 || y < 0 || y > 9)
	{
		x = this.x;
		y = this.y;
	}
	for (int i = 0; i < this.fobiddenPoints.length; i += 2)
		if (this.fobiddenPoints[i] == x && this.fobiddenPoints[i+1] == y)
		{
			x = this.x;
			y = this.y;
		}
	this.x = x;
	this.y = y;
	this.pointsVertical = vertical;
	this.pointsPositive = positive;
	this.repaint();
}

@Override
public void keyReleased(KeyEvent e) {
	// TODO Auto-generated method stub
	
}

@Override
public void keyTyped(KeyEvent e) {
	// TODO Auto-generated method stub
	
}

public void serial_update( int row_num, int col_num) {
	int x = row_num;
	int y = col_num;
	

	this.x = x;
	this.y = y;
	this.pointsVertical = true;
	this.pointsPositive = true;
	this.repaint();
}
 
/** The port we're normally going to use. */
private static final String PORT_NAMES[] = { 
	"/dev/tty.HC-05-DevB"

};
/**
* A BufferedReader which will be fed by a InputStreamReader 
* converting the bytes into characters 
* making the displayed results codepage independent
*/
private BufferedReader input;
/** The output stream to the port */
private OutputStream output;
/** Milliseconds to block while waiting for port open */
private static final int TIME_OUT = 2000;
/** Default bits per second for COM port. */
private static final int DATA_RATE = 9600;

public void initialize() {
        // the next line is for Raspberry Pi and 
        // gets us into the while loop and was suggested here was suggested http://www.raspberrypi.org/phpBB3/viewtopic.php?f=81&t=32186
        //System.setProperty("gnu.io.rxtx.SerialPorts", "/dev/ttyACM0");

CommPortIdentifier portId = null;
Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

//First, Find an instance of serial port as set in PORT_NAMES.
while (portEnum.hasMoreElements()) {
	CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
	for (String portName : PORT_NAMES) {
		if (currPortId.getName().equals(portName)) {
			portId = currPortId;
			break;
		}
	}
}
if (portId == null) {
	System.out.println("Could not find COM port.");
	return;
}

try {
	// open serial port, and use class name for the appName.
	serialPort = (SerialPort) portId.open(this.getClass().getName(),
			TIME_OUT);

	// set port parameters
	serialPort.setSerialPortParams(DATA_RATE,
			SerialPort.DATABITS_8,
			SerialPort.STOPBITS_1,
			SerialPort.PARITY_NONE);

	// open the streams
	input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
	output = serialPort.getOutputStream();

	// add event listeners
	serialPort.addEventListener(this);
	serialPort.notifyOnDataAvailable(true);
} catch (Exception e) {
	System.err.println(e.toString());
}
}

/**
* This should be called when you stop using the port.
* This will prevent port locking on platforms like Linux.
*/
public synchronized void close() {
if (serialPort != null) {
	serialPort.removeEventListener();
	serialPort.close();
}
}

/**
* Handle an event on the serial port. Read the data and print it.
*/
public synchronized void serialEvent(SerialPortEvent oEvent) {
if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
	try {
		String inputLine=input.readLine();
		System.out.println(inputLine);
		String[] values = inputLine.split(":");
		
		int row_num  = Integer.parseInt(values[0], 16);

		int col_num  = Integer.parseInt(values[1], 16);
		serial_update(  row_num,  col_num);
	} catch (Exception e) {
		System.err.println(e.toString());
	}
	
	
}

// Ignore all the other eventTypes, but you should consider the other ones.
}



/*public static void main(String[] args) throws Exception {
	MapUpdatorApplet main = new MapUpdatorApplet();
	main.initialize();
	Thread t=new Thread() {
		public void run() {
			//the following line will keep this app alive for 1000 seconds,
			//waiting for events to occur and responding to them (printing incoming messages to console).
			try {Thread.sleep(1000000);} catch (InterruptedException ie) {}
		}
	};
	t.start();
	System.out.println("Started");
}
*/


}
