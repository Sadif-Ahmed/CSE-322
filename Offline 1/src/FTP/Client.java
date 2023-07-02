package FTP;

import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;

public class Client {

    public static HashSet<String> seenreqids = new HashSet<>();
    public static HashSet<String > seenfulids = new HashSet<>();

    public  static File seenfile = new File("");
    public static File seenreq = new File("");

    public static void write_hashset(HashSet<String> temp,String filepath)
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
    public static void read_hashset(HashSet<String> temp,String filepath)
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
        Socket socket = new Socket("localhost", 6666);
        Socket bsocket = new Socket("localhost", 6666);
        System.out.println("Connection established");
        System.out.println("Remote port: " + socket.getPort());
        System.out.println("Local port: " + socket.getLocalPort());



        // buffers
        ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
        ObjectInputStream in = new ObjectInputStream(socket.getInputStream());

        Scanner tin = new Scanner(System.in);
        String srcpath;
        int logflag = 0;
        String uname = "";
        String input = "";
        int flag = 0;
        HashSet<String> users = new HashSet<String>();
        HashSet<String> active_users = new HashSet<String>();


        // open thread
        Thread worker = new ClientWorker(bsocket);
        worker.start();


        System.out.println("Enter the absolute path of src: ");
        srcpath= tin.nextLine();


        String filepath = srcpath+"\\Client Files";
        File clientfolder = new File(filepath);

        while (true) {
            File temp = new File(filepath);
            if(!temp.exists())
            {
                temp.mkdir();
            }
            if(!socket.isConnected())
            {
                System.out.println("The Server disconnected.Closing client");
                socket.close();
                bsocket.close();
                worker.interrupt();
                break;
            }
            if (logflag == 0) {
                System.out.print("Give Username :");
                uname = tin.nextLine();
                out.writeObject(uname);
                String inputstring = (String) in.readObject();
                if (inputstring.equalsIgnoreCase("Ok")) {
                    System.out.println(uname + " is Connected");
                    logflag = 1;
                    clientfolder= new File(filepath,uname);
                    if(!clientfolder.exists())
                    {
                        clientfolder.mkdir();
                    }
                    File logfolder = new File(clientfolder,"Logs");
                    if(!logfolder.exists())
                    {
                        logfolder.mkdir();
                    }
                    seenfile= new File(logfolder,"seenfile.txt");
                    if(!seenfile.exists())
                    {
                        seenfile.createNewFile();
                    }
                    seenreq =  new File(logfolder,"seenreq.txt");
                    if(!seenreq.exists())
                    {
                        seenreq.createNewFile();
                    }
                    read_hashset(seenreqids,seenreq.getAbsolutePath());
                    read_hashset(seenfulids,seenfile.getAbsolutePath());

                }
                else if (inputstring.equalsIgnoreCase("NotOk")) {
                    System.out.println("You are already logged in another system");
                    socket.close();
                    bsocket.close();
                    worker.interrupt();
                    break;
                }
            }
            else {
                if(!socket.isConnected())
                {
                    System.out.println("The Server disconnected.Closing client");
                    socket.close();
                    bsocket.close();
                    worker.interrupt();
                    break;
                }
                if(flag==1)
                {
                    String trash = (String) in.readObject();
                    flag=0;
                }

                System.out.print("Client -> ");
                input = tin.nextLine();
                if (input.equalsIgnoreCase("ls users")) {
                    out.writeObject(input);
                    users = (HashSet<String>) in.readObject();
                    active_users = (HashSet<String>) in.readObject();
                    System.out.println("The User-list :");
                    for (String i : users) {
                        if (active_users.contains(i)) {
                            System.out.println(i + "(active)");
                        } else {
                            System.out.println(i);
                        }
                    }

                } else if (input.equalsIgnoreCase("logout")) {
                    out.writeObject(input);
                    input = (String) in.readObject();
                    if (input.equalsIgnoreCase("ok")) {
                        System.out.println("Log out Successful");
                        socket.close();
                        bsocket.close();
                        worker.interrupt();
                        break;
                    }
                }
                else if (input.equalsIgnoreCase("ls files"))
                {
                    out.writeObject(input);
                    String [] privatefiles = (String[]) in.readObject();
                    String [] publicfiles = (String[]) in.readObject();
                    System.out.println("List of User's Server Files:");
                    System.out.println("Private Files:");
                    for(int i=0;i<privatefiles.length;i++)
                    {
                        System.out.println((i+1)+". "+privatefiles[i]);
                    }
                    System.out.println("Public Files:");
                    for(int i=0;i<publicfiles.length;i++)
                    {
                        System.out.println((i+1)+". "+publicfiles[i]);
                    }

                }
                else if (input.equalsIgnoreCase("ls allfiles"))
                {
                    out.writeObject(input);

                    String [] userlist = (String[]) in.readObject();
                    String [][] filelist = (String[][]) in.readObject();
                    System.out.println("List of All User's Public Server Files:");
                    System.out.println("Number of users :" + userlist.length);
                    for(int i=0;i< userlist.length;i++)
                    {
                        System.out.println("Files of user: "+ userlist[i]);
                        for(int j=0;j<filelist[i].length;j++)
                        {
                            System.out.println((j+1)+". "+filelist[i][j]);
                        }
                    }
                }
                else if(input.equalsIgnoreCase("req file"))
                {

                    System.out.println("Give Description:");
                    String file_desc = tin.nextLine();
                    out.writeObject(input);
                    out.writeObject(file_desc);
                    String conf = (String) in.readObject();
                    System.out.println(conf);
                }
                else if(input.equalsIgnoreCase("view msgs"))
                {
                    out.writeObject(input);
                    HashMap<Integer , String> filereqsender = (HashMap<Integer, String>) in.readObject();
                    HashMap<Integer , String> filerequests = (HashMap<Integer, String>) in.readObject();
                    HashMap<Integer , String> fulfilledreq = (HashMap<Integer, String>) in.readObject();
                    HashMap<Integer , String> reqfullfiller = (HashMap<Integer, String>) in.readObject();
                    //System.out.println(filerequests);
                    //System.out.println(seenreqids);
                    System.out.println("Unseen Messages: ");
                    for (int i: filerequests.keySet())
                    {
                        if (!seenreqids.contains(Integer.toString(i)))
                        {
                            System.out.println("Request id: "+ (i) +" Sender: "+filereqsender.get(i) + " File Description: " +filerequests.get(i));
                            seenreqids.add(Integer.toString(i));
                        }
                        if(!seenfulids.contains(reqfullfiller.get(i)+" "+fulfilledreq.get(i)) && fulfilledreq.containsKey(i) && filereqsender.get(i).equalsIgnoreCase(uname))
                        {
                            System.out.println("The request with id: "+i+" has been fulfilled by user: "+reqfullfiller.get(i)+" with the file: "+fulfilledreq.get(i));
                            seenfulids.add(reqfullfiller.get(i)+" "+fulfilledreq.get(i));

                        }
                    }
                    write_hashset(seenreqids,seenreq.getAbsolutePath());
                    write_hashset(seenfulids,seenfile.getAbsolutePath());

                }
                else if(input.equalsIgnoreCase("view allmsgs"))
                {
                    out.writeObject(input);
                    HashMap<Integer , String> filereqsender = (HashMap<Integer, String>) in.readObject();
                    HashMap<Integer , String> filerequests = (HashMap<Integer, String>) in.readObject();
                    HashMap<Integer , String> fulfilledreq = (HashMap<Integer, String>) in.readObject();
                    HashMap<Integer , String> reqfullfiller = (HashMap<Integer, String>) in.readObject();

                    //System.out.println(filerequests);
                    //System.out.println(seenreqids);
                    System.out.println("All Messages: ");
                    for (int i: filerequests.keySet())
                    {
                            System.out.println("Request id: "+ (i) +" Sender: "+filereqsender.get(i) + " File Description: " +filerequests.get(i));
                            seenreqids.add(Integer.toString(i));
                        if(fulfilledreq.containsKey(i) && filereqsender.get(i).equalsIgnoreCase(uname))
                        {
                            System.out.println("The request with id: "+i+" has been fulfilled by "+reqfullfiller.get(i)+" with file name: "+fulfilledreq.get(i));
                            seenfulids.add(reqfullfiller.get(i)+" "+fulfilledreq.get(i));
                        }

                    }
                    write_hashset(seenreqids,seenreq.getAbsolutePath());
                    write_hashset(seenfulids,seenfile.getAbsolutePath());


                }
                else if(input.equalsIgnoreCase("get file"))
                {
                    out.writeObject(input);
                    System.out.println("Give Filename: ");
                    String filename = tin.nextLine();
                    System.out.println("Give File source(Username): ");
                    String filesrc = tin.nextLine();
                    out.writeObject(filename);
                    out.writeObject(filesrc);
                    String message = (String) in.readObject();
                    System.out.println(message);
                    if(message.equalsIgnoreCase("File Exists."))
                    {
                        long filesize = (long) in.readObject();
                        System.out.println("The file size is : " + filesize + " bytes");
                        long numchunks = (long) in.readObject();
                        System.out.println("The file will be sent in " + numchunks + " chunks");
                        byte[] data = new byte[(int)filesize];
                        byte[] holder = new byte[(int)Server.MAX_CHUNK_SIZE] ;
                        File downfile = new File(clientfolder,filename);
                        downfile.createNewFile();
                        if(numchunks==1)
                        {

                            data = (byte[]) in.readObject();
                            Files.write(Paths.get(downfile.getAbsolutePath()), data);
                        }
                        else
                        {
                            for(int i=0;i<numchunks-1;i++)
                            {

                                holder = (byte[]) in.readObject();
                                for(int j=0;j<Server.MAX_CHUNK_SIZE;j++)
                                {
                                    data[i*Server.MAX_CHUNK_SIZE+j]=holder[j];
                                }
                            }

                            holder = (byte[]) in.readObject();

                            for(long i=(numchunks-1)*Server.MAX_CHUNK_SIZE,j=0;i<filesize;i++,j++)
                            {
                                data[(int) i]=holder[(int) j];
                            }
                            Files.write(Paths.get(downfile.getAbsolutePath()), data);
                        }
                        message = (String) in.readObject();
                        System.out.println(message);
                    }

                }
                else if(input.equalsIgnoreCase("ls localfiles"))
                {
                    String [] filelist = clientfolder.list();
                    System.out.println("List of User's Local Files:");
                    int idx=1;
                    for(int i=0;i<filelist.length;i++)
                    {
                        if(!filelist[i].equalsIgnoreCase("Logs"))
                        {
                            System.out.println((idx++) + ". " + filelist[i]);
                        }
                    }
                }
                else if(input.equalsIgnoreCase("upload file"))
                {
                    out.writeObject(input);
                    System.out.println("Are you fulfilling a previous file request?(Y/N)");
                    String choice = tin.nextLine();
                    out.writeObject(choice);
                    if(choice.equalsIgnoreCase("Y"))
                    {
                        HashMap<Integer , String> filereqsender = (HashMap<Integer, String>) in.readObject();
                        HashMap<Integer , String> filerequests = (HashMap<Integer, String>) in.readObject();
                        //System.out.println(filerequests);
                        //System.out.println(seenreqids);
                        System.out.println("All File Requests: ");
                        for (int i: filerequests.keySet())
                        {
                            System.out.println("Request id: "+ (i) +" Sender: "+filereqsender.get(i) + " File Description: " +filerequests.get(i));
                        }
                        filerequests.clear();
                        System.out.println("Enter the file request id you are fulfilling: ");
                        String reqfileid = tin.nextLine();
                        out.writeObject(reqfileid);
                    }
                    else if(choice.equalsIgnoreCase("N"))
                    {

                    }
                    System.out.println("Give File Name: ");
                    String fname = tin.nextLine();
                    String privacy;
                    if(choice.equalsIgnoreCase("N"))
                    {
                        System.out.println("Public or Private?");
                        privacy = tin.nextLine();
                    }
                    else
                    {
                        privacy="Public";
                    }
                    File upfile = new File(clientfolder.getAbsolutePath(), fname);
                    if(!upfile.exists())
                    {
                        System.out.println("The file is absent in client system.");
                        out.writeObject("not ok");
                    }
                    else
                    {
                        out.writeObject("ok");
                        out.writeObject(fname);
                        out.writeObject(privacy);
                        out.writeObject(upfile.length());
                        String mes= (String) in.readObject();
                        if(mes.equalsIgnoreCase("ok"))
                        {
                            String dup=(String) in.readObject();
                            if(dup.equalsIgnoreCase("newfile")) {

                                int chunksize = (int) in.readObject();
                                int fileid = (int) in.readObject();
                                System.out.println("the File Id is : " + fileid);
                                System.out.println("The Server allocated chunk size is: " + chunksize);

                                long num_of_chunks = (upfile.length() / chunksize) + 1;
                                out.writeObject(num_of_chunks);
                                byte[] data = Files.readAllBytes(upfile.toPath());
                                byte[] holder = new byte[chunksize];
                                flag = 0;

                                try {

                                    if (num_of_chunks == 1) {
                                        out.writeObject(data);
                                        socket.setSoTimeout(30000);
                                        String conf = (String) in.readObject();
                                        //System.out.println(conf + " 1");
                                        socket.setSoTimeout(0);
                                    } else {
                                        for (int i = 0; i < num_of_chunks - 1; i++) {
                                            if (flag == 0) {
                                                for (int j = 0; j < chunksize; j++) {
                                                    holder[j] = data[i * chunksize + j];
                                                }
                                                out.writeObject(holder);
                                                socket.setSoTimeout(30000);
                                                String conf = (String) in.readObject();
                                                if (!conf.equalsIgnoreCase("ok")) {
                                                    flag = 1;
                                                }
                                                //System.out.println(conf+"  "+(i+1));
                                                socket.setSoTimeout(0);
                                                holder = new byte[chunksize];
                                            }
                                        }
                                        if (flag == 0) {

                                            for (long i = (num_of_chunks - 1) * chunksize, j = 0; i < upfile.length(); i++, j++) {
                                                holder[(int) j] = data[(int) i];
                                            }
                                            out.writeObject(holder);

                                        }

                                    }


                                } catch (SocketTimeoutException e) {
                                    flag = 1;
                                    System.out.println("Upload Interrupted.Time out Exception.");
                                    socket.setSoTimeout(0);
                                }
                                if (flag == 0) {
                                    out.writeObject("File Upload Complete");
                                    String message = (String) in.readObject();
                                    System.out.println(message);
                                    socket.setSoTimeout(0);
                                }
                                if (flag == 1) {
                                    System.out.println("Upload Interrupted.");
                                    socket.setSoTimeout(0);
                                }
                            } else if (dup.equalsIgnoreCase("duplicate")) {
                                System.out.println("A File with the same name already exists in the Server.");
                            }


                        }
                        else if(mes.equalsIgnoreCase("not ok"))
                        {
                            System.out.println("The File Size Crosses the Maximum File Buffer Size.");
                        }
                        }

                } else if (input.equalsIgnoreCase("delete file")) {
                    out.writeObject(input);
                    System.out.println("Give Filename: ");
                    String filename = tin.nextLine();
                    out.writeObject(filename);
                    String message = (String) in.readObject();
                    System.out.println(message);
                    if(message.equalsIgnoreCase("File Exists."))
                    {
                        System.out.println("Do you really want to delete the file in server?(Y/N)");
                        String choice = tin.nextLine();
                        out.writeObject(choice);
                        if(choice.equalsIgnoreCase("Y"))
                        {
                            String conf = (String) in.readObject();
                            System.out.println(conf);
                        }

                    }

                } else {
                    System.out.println("Invalid Command Try Again");
                }
            }

        }
    }
}

