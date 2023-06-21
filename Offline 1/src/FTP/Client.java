package FTP;

import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;

public class Client {

    public static void main(String[] args) throws IOException, ClassNotFoundException {
        Socket socket = new Socket("localhost", 6666);
        System.out.println("Connection established");
        System.out.println("Remote port: " + socket.getPort());
        System.out.println("Local port: " + socket.getLocalPort());

        // buffers
        ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
        ObjectInputStream in = new ObjectInputStream(socket.getInputStream());

        Scanner tin = new Scanner(System.in);
        int logflag = 0;
        String uname = "";
        String input = "";
        HashSet<String> users = new HashSet<String>();
        HashSet<String> active_users = new HashSet<String>();
        HashSet<Integer> seenreqids = new HashSet<>();
        String filepath = "E:\\CSE-322\\Offline 1\\src\\Client Files";
        //
        File clientfolder = new File(filepath);

        while (true) {
            if(!socket.isConnected())
            {
                System.out.println("The Server disconnected.Closing client");
                socket.close();
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
                } else if (inputstring.equalsIgnoreCase("NotOk")) {
                    System.out.println("You are already logged in another system");
                    socket.close();
                    break;
                }
            }
            else {
                if(!socket.isConnected())
                {
                    System.out.println("The Server disconnected.Closing client");
                    socket.close();
                    break;
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
                    input = "";
                } else if (input.equalsIgnoreCase("logout")) {
                    out.writeObject(input);
                    input = (String) in.readObject();
                    if (input.equalsIgnoreCase("ok")) {
                        System.out.println("Log out Successful");
                        socket.close();
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
                    HashMap<Integer , String> filerequests = (HashMap<Integer, String>) in.readObject();
                  System.out.println(filerequests);
                    System.out.println(seenreqids);
                    System.out.println("Unseen Messages: ");
                    for (int i: filerequests.keySet())
                    {
                        if (!seenreqids.contains(i))
                        {
                            String [] str = filerequests.get(i).split(" ",2);
                            System.out.println("Request id: "+ (i) +" Sender: "+str[0] + " File Description: " +str[1]);
                            seenreqids.add(i);
                        }
                    }
                    filerequests.clear();
                }
                else if(input.equalsIgnoreCase("view allmsgs"))
                {
                    out.writeObject(input);
                    HashMap<Integer , String> filerequests = (HashMap<Integer, String>) in.readObject();
                    System.out.println(filerequests);
                    System.out.println(seenreqids);
                    System.out.println("All Messages: ");
                    for (int i: filerequests.keySet())
                    {

                            String [] str = filerequests.get(i).split(" ",2);
                            System.out.println("Request id: "+ (i) +" Sender: "+str[0] + " File Description: " +str[1]);
                            seenreqids.add(i);

                    }
                    filerequests.clear();
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
                else if(input.equalsIgnoreCase("put file"))
                {
                    System.out.println("Give File Name: ");
                    String fname = tin.nextLine();
                    System.out.println("Public or Private?");
                    String privacy = tin.nextLine();
                    File upfile = new File(clientfolder.getAbsolutePath(), fname);
                    if(!upfile.exists())
                    {
                        System.out.println("The file is absent in client system.");
                    }
                    else
                    {
                        out.writeObject(input);
                        out.writeObject(fname);
                        out.writeObject(privacy);
                        out.writeObject(upfile.length());
                        int chunksize = (int) in.readObject();
                        int fileid = (int) in.readObject();
                        System.out.println("the File Id is : " + fileid);
                        System.out.println("The Server allocated chunk size is: " + chunksize);

                        long num_of_chunks = (upfile.length() / chunksize) + 1 ;
                        out.writeObject(num_of_chunks);
                        byte[] data = Files.readAllBytes(upfile.toPath());
                        byte[] holder = new byte[chunksize];

                        if(num_of_chunks==1)
                        {
                            out.writeObject(data);
                        }
                        else
                        {
                            for(int i=0;i<num_of_chunks-1;i++)
                            {
                                for(int j=0;j<chunksize;j++)
                                {
                                    holder[j]=data[i*chunksize+j];
                                }
                                out.writeObject(holder);
                                holder = new byte[chunksize];
                            }

                            for(long i=(num_of_chunks-1)*chunksize,j=0;i<upfile.length();i++,j++)
                            {
                                holder[(int) j]=data[(int) i];
                            }
                            out.writeObject(holder);
                    }
                        out.writeObject("File Upload Complete");
                    }
                }
                else if(input.equalsIgnoreCase("ls localfiles"))
                {
                    String [] filelist = clientfolder.list();
                    System.out.println("List of User's Local Files:");
                    for(int i=0;i<filelist.length;i++)
                    {
                        System.out.println((i+1)+". "+filelist[i]);
                    }
                }

                else {
                    System.out.println("Invalid Command Try Again");
                }
            }

        }
    }
}

