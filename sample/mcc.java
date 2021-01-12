import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.InetSocketAddress;

public class mcc
{
    public static void main(String[] args) throws Exception
    {
        String group = "225.0.0.255";
        int port = 11111;
        String local = "192.168.43.104";
        if(args.length > 0)
            group = args[0];
        if(args.length > 1)
            port = Integer.parseInt(args[1]);
        if(args.length > 2)
            local = args[2];
            
        System.out.println("using configuration " + group + ":" + port + " at " + local);
        InetAddress addr = InetAddress.getByName(group);
        MulticastSocket ms = null;
        try
        {
            ms = new MulticastSocket(new InetSocketAddress(local, port));
            System.out.println(ms.getInterface());
            System.out.println(ms.getInetAddress());
            ms.joinGroup(new InetSocketAddress(addr, port), NetworkInterface.getByInetAddress(InetAddress.getByName(local)));
            byte[] buff = new byte[2048];
            while(true)
            {
                DatagramPacket dp = new DatagramPacket(buff, buff.length);
                ms.receive(dp);
                System.out.println(new String(dp.getData(), 0, dp.getLength()));
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
                ms.leaveGroup(addr);
                ms.close();
            }
        }
    }
}