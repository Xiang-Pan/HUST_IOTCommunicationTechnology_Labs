import java.io.IOException;
//import java.io.*;
import java.util.Scanner;
import net.tinyos.message.*;
import net.tinyos.packet.*;
import net.tinyos.util.*;

public class BlinkToRadio implements MessageListener {
	private MoteIF moteIF;
	int counter = 0;
	int nodeid = 0;

	public BlinkToRadio(MoteIF moteIF) {
		this.moteIF = moteIF;
		this.moteIF.registerListener(new BlinkToRadioMsg(), this);
	/*}*/

	/*public void sendPackets() {*/
		BlinkToRadioMsg payload = new BlinkToRadioMsg();
		Scanner in = new Scanner(System.in);

		try {
			while(true) {
				System.out.println("Input the nodeid and counter(like 1 2): ");
				if(in.hasNextInt()){
					nodeid = in.nextInt();
					if(nodeid == 0) break;	
					counter = in.nextInt();				
				}
				System.out.println("Sending to " + nodeid + " number is : " + counter);
				payload.set_counter(counter);
				payload.set_nodeid(nodeid);
				moteIF.send(4, payload);
				try {
					Thread.sleep(1000);
				}catch(InterruptedException exception){}
			}
		}catch(IOException exception){
			System.err.println("Exception thrown when sending packets. Exiting.");
        	System.err.println(exception);
		}
	}

	public void messageReceived(int to, Message message) {
		BlinkToRadioMsg msg = (BlinkToRadioMsg)message;
		System.out.println("Received packet to: " + to + "  nodeid: " + 
			msg.get_nodeid() + "  counter: " + msg.get_counter());

	}

	private static void usage() {
		System.err.println("usage: BlinkToRadio [-comm <source>]"); 
	}

	public static void main(String[] args) throws Exception {
		String source = null;
		if(args.length == 2) {
			if(!args[0].equals("-comm")) {
				usage();
				System.exit(1);
			}
			source = args[1];
		}
		else if(args.length != 0) {
			usage();
			System.exit(1);
		}

		PhoenixSource phoenix;

		if(source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		}
		else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}

 		MoteIF mif = new MoteIF(phoenix);
 		BlinkToRadio radio = new BlinkToRadio(mif);
 		//radio.sendPackets(); 
	}
}