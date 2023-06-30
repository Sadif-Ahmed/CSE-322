package FTP;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.net.SocketException;
import java.util.Date;

public class ClientWorker extends Thread  {
    Socket socket;


    public ClientWorker(Socket socket)
    {
        this.socket = socket;
    }

    public void run()
    {

        try {
            ObjectInputStream in = new ObjectInputStream(this.socket.getInputStream());
            String message;

            while (!Thread.currentThread().isInterrupted())
            {
                try {

                        message = (String) in.readObject();
                        System.out.println("Broadcast Message: "+message);
                        System.out.print("Client->");


                }
                catch (SocketException e)
                {
                    break;
                }

            }
        } catch (IOException |ClassNotFoundException  e) {
          // e.printStackTrace();

        }
    }

}
