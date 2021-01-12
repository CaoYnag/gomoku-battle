import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.MulticastSocket;
import java.net.InetSocketAddress;

public class mcs
{
    public static void main(String[] args) throws Exception
    {
        String group = "225.0.0.255";
        int port = 11111;
        String local = "192.168.43.104";
        String msg = "Hello, World!";
        if(args.length > 0)
            group = args[0];
        if(args.length > 1)
            port = Integer.parseInt(args[1]);
        if(args.length > 2)
            local = args[2];
        if(args.length > 3)
            msg = args[3];

        System.out.println("using configuration " + group + ":" + port + " at " + local);
        InetAddress addr = InetAddress.getByName(group);
        MulticastSocket ms = null;
        try
        {
            ms = new MulticastSocket(new InetSocketAddress(local, port + 1));
            ms.setTimeToLive(100);
            //ms.joinGroup(addr);
            int cnt = 0;
            while(true)
            {
                String msg_snd = "the " + (cnt++) + "th msg: " + msg + "\n";
                byte[] buff = msg_snd.getBytes();
                DatagramPacket dp = new DatagramPacket(buff, buff.length, addr, port);
                ms.send(dp);
                System.out.print(msg_snd);
                Thread.sleep(1000);
            }
        } 
        catch(Exception e)
        {
            e.printStackTrace();
        }
        finally
        {
            if(ms != null)
            {
                //ms.leaveGroup(addr);
                ms.close();
            }
        }
    }
}