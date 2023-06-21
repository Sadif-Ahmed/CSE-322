package Threading;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;

public class Server {
    public static HashSet<String> users=new HashSet<>();
    public static HashSet<String> active_users=new HashSet<>();
    public static String orignalPath = "E:\\Offline1\\src\\Server Files";
    public static File workingDir ;


    public static int filereqcount = 0;

    public static HashMap<Integer , String> filerequests = new HashMap<Integer,String>() ;

    public static HashMap<Integer,String> filereqsender = new HashMap<>();

    public static  HashMap<String,Socket> activeusersockets = new HashMap<>();

    public static HashMap<Integer,String> fileuploadinfo = new HashMap<>();
    public static HashMap<Integer,String> fileuploaderinfo = new HashMap<>();

    public static int fileuploadcount=0;

    public static int MAX_BUFFER_SIZE;

    public static int MAX_CHUNK_SIZE=5000;

    public static int MIN_CHUNK_SIZE=1000;
   public static void write_user_list(HashSet<String> temp,String filepath)
    {
        try {
            FileWriter myWriter = new FileWriter(filepath);
            for(String i : temp) {
                myWriter.write(i);
                myWriter.write("\n");
            }
            myWriter.close();
        } catch (IOException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }

    public static void read_user_list(HashSet<String> temp,String filepath)
    {
        try {
            File myObj = new File(filepath);
            Scanner myReader = new Scanner(myObj);
            while (myReader.hasNextLine()) {
                String data = myReader.nextLine();
                if(!data.equalsIgnoreCase("")) {
                    temp.add(data);
                }
            }
            myReader.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }

    public static void main(String[] args) throws IOException, ClassNotFoundException {
        ServerSocket welcomeSocket = new ServerSocket(6666);
        String userfilepath = "E:\\Offline1\\src\\user_list.txt";
        Server.read_user_list(users, userfilepath);

        while(true) {

            System.out.println("Waiting for connection...");
            Socket socket = welcomeSocket.accept();
            System.out.println("Connection established");
            ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
            ObjectInputStream in = new ObjectInputStream(socket.getInputStream());
            String username = (String) in.readObject();
            if(!active_users.contains(username)) {
                System.out.println("Client Connected with username: " + username);
                synchronized (active_users)
                {
                    active_users.add(username);
                }
                activeusersockets.put(username,socket);
                out.writeObject("Ok");
                if(!users.contains(username))
                {
                    synchronized (users)
                    {
                        users.add(username);
                        write_user_list(users,userfilepath);
                        workingDir = new File (orignalPath,username);
                        if(workingDir.mkdir())
                        {
                            File tempdir;
                            tempdir=new File(workingDir.getAbsolutePath(),"Public");
                            tempdir.mkdir();
                            tempdir=new File(workingDir.getAbsolutePath(),"Private");
                            tempdir.mkdir();
                            System.out.println("Directory Created for " + username);
                        }
                    }

                }
                else
                {
                    workingDir = new File (orignalPath,username);
                }
                // open thread

                //System.out.println(workingDir);
                Thread worker = new Worker(socket,in,out,username,workingDir);
                worker.start();
            }
            else
            {
                out.writeObject("NotOk");
                System.out.println("User Already Active");
                socket.close();
            }


        }

    }
}
