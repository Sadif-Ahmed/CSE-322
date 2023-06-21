package Threading;

import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;

public class Worker extends Thread {
    Socket socket;
    ObjectOutputStream out;
    ObjectInputStream in;
    String username;
    File workingDir;
    public Worker(Socket socket,ObjectInputStream in,ObjectOutputStream out,String username,File workingDir)
    {
        this.socket = socket;
        this.in=in;
        this.out=out;
        this.username=username;
        this.workingDir=workingDir;
    }

    public void run()
    {


        String receivedinput="";

        // buffers

            while (true)
            {
                try
                {
                    System.out.println("Waiting For Command...");
                    receivedinput = (String) in.readObject();
                    System.out.println("Received Command : " + receivedinput + " From " + username);
                    if (receivedinput.equalsIgnoreCase("ls users"))
                    {

                        synchronized (Server.users){
                            out.writeObject(Server.users);
                        }
                        synchronized (Server.active_users)
                        {
                            out.writeObject(Server.active_users);
                        }

                        receivedinput = "";
                    }
                    else if (receivedinput.equalsIgnoreCase("logout"))
                    {
                        Server.active_users.remove(username);
                        Server.activeusersockets.remove(username);
                        System.out.println(username + " has logged out.");
                        out.writeObject("ok");
                        socket.close();
                        break;
                    }
                    else if(receivedinput.equalsIgnoreCase("ls files"))
                    {
                        File Privatefiles = new File(workingDir,"Private");
                        File Publicfiles = new File(workingDir,"Public");
                        out.writeObject(Privatefiles.list());
                        out.writeObject(Publicfiles.list());
                        receivedinput = "";
                    } else if (receivedinput.equalsIgnoreCase("ls allfiles")) {
                        File parentdir = workingDir.getParentFile();
                        File tempdir;
                        String [] userlist = parentdir.list();
                        String [][] filelist = new String[userlist.length][];
                        int num_users=userlist.length;
                        out.writeObject(userlist);
                       for(int i=0;i<num_users;i++)
                       {
                           tempdir = new File(parentdir.getAbsolutePath()+"/"+userlist[i]+"/Public");
                           filelist[i] = tempdir.list();
                       }
                       out.writeObject(filelist);
                    }
                    else if(receivedinput.equalsIgnoreCase("req file"))
                    {
                        String filedesc=(String) in.readObject();
                        String message = username + " has requested the file with description: " + filedesc;
                        System.out.println(message);
                        filedesc = username + " " + filedesc;
                        Server.filereqsender.put(Server.filereqcount,username);
                        Server.filerequests.put(Server.filereqcount,filedesc);
                        Server.filereqcount++;
                        System.out.println(Server.filerequests);
                        System.out.println(Server.filereqsender);
                        out.writeObject(message);

                    }
                    else if (receivedinput.equalsIgnoreCase("view msgs"))
                    {
                        System.out.println(Server.filerequests);
                        out.writeObject(new HashMap<>(Server.filerequests));
                    }
                    else if (receivedinput.equalsIgnoreCase("view allmsgs"))
                    {
                        System.out.println(Server.filerequests);
                        out.writeObject(new HashMap<>(Server.filerequests));
                    }
                    else if(receivedinput.equalsIgnoreCase("get file"))
                    {
                        String filename = (String) in.readObject();
                        String fileuname = (String) in.readObject();
                        String filepath;
                        File inputfile;
                        if(!username.equalsIgnoreCase(fileuname))
                        {
                            filepath = workingDir.getParent();
                            filepath = filepath +"/"+ fileuname + "/Public" ;
                            inputfile = new File(filepath,filename) ;
                            if(inputfile.exists()) {
                                out.writeObject("File Exists.");
                            }
                            else
                            {
                                out.writeObject("File does not Exist.");
                                continue;
                            }
                        }
                        else
                        {
                            inputfile = new File(workingDir.getAbsolutePath()+"/Public",filename);
                            if(!inputfile.exists())
                            {
                                inputfile = new File(workingDir.getAbsolutePath()+"/Private",filename);
                                if (!inputfile.exists())
                                {
                                    out.writeObject("File does not Exist.");
                                    continue;
                                }
                            }
                            out.writeObject("File Exists.");
                        }
                        out.writeObject(inputfile.length());
                        long num_of_chunks = (inputfile.length() / Server.MAX_CHUNK_SIZE) + 1 ;
                        out.writeObject(num_of_chunks);
                        byte[] data = Files.readAllBytes(inputfile.toPath());
                        byte[] holder = new byte[Server.MAX_CHUNK_SIZE];
                        if(num_of_chunks==1)
                        {
                            out.writeObject(data);
                        }
                        else
                        {
                            for(int i=0;i<num_of_chunks-1;i++)
                            {
                                for(int j=0;j<Server.MAX_CHUNK_SIZE;j++)
                                {
                                    holder[j]=data[i*Server.MAX_CHUNK_SIZE+j];
                                }
                                out.writeObject(holder);
                                holder = new byte[Server.MAX_CHUNK_SIZE];
                            }
                            for(long i=(num_of_chunks-1)*Server.MAX_CHUNK_SIZE,j=0;i<inputfile.length();i++,j++)
                            {
                                holder[(int) j]=data[(int) i];
                            }
                            out.writeObject(holder);

                        }
                        out.writeObject("File Download Complete");
                    }
                    else if(receivedinput.equalsIgnoreCase("put file"))
                    {
                        String fname = (String) in.readObject();
                        String privacy = (String) in.readObject();
                        long fsize = (long) in.readObject();
                        System.out.println(username + " is trying to upload a " + privacy + " file named " + fname + " and of size " + fsize+" bytes");
                        Server.fileuploadinfo.put(Server.fileuploadcount, fname);
                        Server.fileuploaderinfo.put(Server.fileuploadcount, username);
                        int chunksize = (int) Math.floor(Math.random() * (Server.MAX_CHUNK_SIZE - Server.MIN_CHUNK_SIZE + 1) + Server.MIN_CHUNK_SIZE);
                        out.writeObject(chunksize);
                        out.writeObject(Server.fileuploadcount);
                        Server.fileuploadcount++;

                        long numchunks = (long) in.readObject();
                        System.out.println("The file will be received in " + numchunks + " chunks");
                        byte[] data = new byte[(int)fsize];
                        byte[] holder = new byte[chunksize];
                        File downfile = new File(workingDir.getAbsolutePath()+"/"+privacy,fname);
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
                                for(int j=0;j<chunksize;j++)
                                {
                                    data[i*chunksize+j]=holder[j];
                                }
                            }
                            holder = (byte[]) in.readObject();
                            for(long i=(numchunks-1)*chunksize,j=0;i<fsize;i++,j++)
                            {
                                data[(int) i]=holder[(int) j];
                            }
                            Files.write(Paths.get(downfile.getAbsolutePath()), data);
                        }
                        String message = (String) in.readObject();
                        System.out.println(message);
                    }


                    receivedinput = "";
                }
                catch (IOException |  ClassNotFoundException  e)
                {
                    e.printStackTrace();
                    Server.active_users.remove(username);
                    Server.activeusersockets.remove(username);
                    System.out.println(username+" has logged out.");
                    break;
                }


            }
        }

}
