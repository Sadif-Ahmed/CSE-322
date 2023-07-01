package FTP;

import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.HashSet;

public class Worker extends Thread {
    Socket socket;
    Socket bsocket;
    ObjectOutputStream out;
    ObjectInputStream in;
    String username;
    File workingDir;
    int fileid;


    public Worker(Socket socket,ObjectInputStream in,ObjectOutputStream out,String username,File workingDir,Socket bsocket)
    {
        this.socket = socket;
        this.in=in;
        this.out=out;
        this.username=username;
        this.workingDir=workingDir;
        this.bsocket=bsocket;
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
                            out.writeObject(new HashSet<>(Server.users));
                        }
                        synchronized (Server.active_users)
                        {
                            out.writeObject(new HashSet<>(Server.active_users));
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
                        int filereqid=Server.filereqcount++;
                        Server.filereqsender.put(filereqid,username);
                        Server.filerequests.put(filereqid,filedesc);
                        //System.out.println(Server.filerequests);
                        //System.out.println(Server.filereqsender);
                        Server.write_hashmap(Server.filerequests,Server.reqlistpath);
                        Server.write_hashmap(Server.filereqsender,Server.reqsenderpath);
                        out.writeObject(message);
                        for(String i : Server.activeusersockets.keySet())
                        {
                            ObjectOutputStream tempout = new ObjectOutputStream(Server.activeusersockets.get(i).getOutputStream());
                            tempout.writeObject(message);
                            tempout.writeObject("reqbrd");
                            tempout.writeObject(filereqid);
                        }

                    }
                    else if (receivedinput.equalsIgnoreCase("view msgs"))
                    {
                        //System.out.println(Server.filerequests);
                        out.writeObject(new HashMap<>(Server.filereqsender));
                        out.writeObject(new HashMap<>(Server.filerequests));
                        out.writeObject(new HashMap<>(Server.fulfilledrequests));
                        out.writeObject(new HashMap<>(Server.reqfulfiller));

                    }
                    else if (receivedinput.equalsIgnoreCase("view allmsgs"))
                    {
                        //System.out.println(Server.filerequests);
                        out.writeObject(new HashMap<>(Server.filereqsender));
                        out.writeObject(new HashMap<>(Server.filerequests));
                        out.writeObject(new HashMap<>(Server.fulfilledrequests));
                        out.writeObject(new HashMap<>(Server.reqfulfiller));
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

                    else if(receivedinput.equalsIgnoreCase("upload file"))
                    {
                        String choice = (String) in.readObject();
                        String reqfileid="";
                        if(choice.equalsIgnoreCase("Y"))
                        {
                            out.writeObject(new HashMap<>(Server.filereqsender));
                            out.writeObject(new HashMap<>(Server.filerequests));
                            reqfileid = (String) in.readObject();
                            System.out.println(username + " is fulfilling a request with req id :" +reqfileid);
                        }
                        else if(choice.equalsIgnoreCase("N"))
                        {

                        }
                        String mes = (String) in.readObject();
                        if(mes.equalsIgnoreCase("ok"))
                        {

                        String fname = (String) in.readObject();
                        String privacy = (String) in.readObject();
                        long fsize = (long) in.readObject();
                        if (fsize<=Server.MAX_BUFFER_SIZE)
                        {
                            out.writeObject("ok");
                            File downfile = new File(workingDir.getAbsolutePath()+"/"+privacy,fname);
                            File mirrorfile ;
                            if(privacy.equalsIgnoreCase("Public"))
                            {
                                mirrorfile = new File(workingDir.getAbsolutePath()+"/"+"Private",fname);
                            }
                            else
                            {
                                mirrorfile = new File(workingDir.getAbsolutePath()+"/"+"Public",fname);
                            }
                            if(!(downfile.exists() || mirrorfile.exists()))
                            {
                            out.writeObject("newfile");

                            fileid=Server.fileuploadcount++;
                            System.out.println(username + " is trying to upload a " + privacy + " file named " + fname + " and of size " + fsize+" bytes assigned fileid "+Server.fileuploadcount);
                            int chunksize = (int) Math.floor(Math.random() * (Server.MAX_CHUNK_SIZE - Server.MIN_CHUNK_SIZE + 1) + Server.MIN_CHUNK_SIZE);
                            out.writeObject(chunksize);
                            out.writeObject(Server.fileuploadcount);

                            long numchunks = (long) in.readObject();
                            System.out.println("The file will be received in " + numchunks + " chunks");
                            byte [] data = new byte[(int)fsize];
                            Server.datamap.put(fileid,new byte[(int)fsize] );
                            byte[] holder = new byte[chunksize];
                            if(numchunks==1)
                            {

                                data = (byte[]) in.readObject();
                                Server.datamap.put(fileid,data);
                                out.writeObject("ok");
                            }
                            else
                            {
                                for(int i=0;i<numchunks-1;i++)
                                {
                                    holder = (byte[]) in.readObject();
                                    out.writeObject("ok");
                                    for(int j=0;j<chunksize;j++)
                                    {
                                        data[i*chunksize+j]=holder[j];
                                    }
                                    Server.datamap.put(fileid,data);

                                }
                                holder = (byte[]) in.readObject();
                                for(long i=(numchunks-1)*chunksize,j=0;i<fsize;i++,j++)
                                {
                                    data[(int) i]=holder[(int) j];
                                }
                                Server.datamap.put(fileid,data);

                            }
                            downfile.createNewFile();
                            Files.write(Paths.get(downfile.getAbsolutePath()), Server.datamap.get(fileid));
                            String message = (String) in.readObject();
                            System.out.println(message);

                            if(fsize==downfile.length())
                            {
                                out.writeObject("File Successfully Uploaded.");
                                Server.fileuploadinfo.put(fileid, fname);
                                Server.fileuploaderinfo.put(fileid, username);
                                Server.write_hashmap(Server.fileuploadinfo,Server.filelistpath);
                                Server.write_hashmap(Server.fileuploaderinfo,Server.uploader_listpath);
                                if(choice.equalsIgnoreCase("Y"))
                                {
                                    Server.fulfilledrequests.put(Integer.parseInt(reqfileid),fname);
                                    Server.reqfulfiller.put(Integer.parseInt(reqfileid),username);
                                    Server.write_hashmap(Server.fulfilledrequests,Server.fulfilledreqpath);
                                    Server.write_hashmap(Server.reqfulfiller,Server.reqfulfillerpath);

                                    ObjectOutputStream tempout = new ObjectOutputStream(Server.activeusersockets.get(Server.filereqsender.get(Integer.parseInt(reqfileid))).getOutputStream());
                                    message = "Your request with id: "+reqfileid+" has been fulfilled by user: "+Server.reqfulfiller.get(Integer.parseInt(reqfileid))+" with the file: "+Server.fulfilledrequests.get(Integer.parseInt(reqfileid));
                                    tempout.writeObject(message);
                                    tempout.writeObject("filebrd");
                                    tempout.writeObject(Server.reqfulfiller.get(Integer.parseInt(reqfileid))+" "+Server.fulfilledrequests.get(Integer.parseInt(reqfileid)));

                                }
                                }
                            else
                            {
                                out.writeObject("Something Went Wrong.");
                                Server.datamap.remove(fileid);
                            }
                            }
                            else
                            {
                                out.writeObject("duplicate");
                            }
                        }
                        else
                        {
                            out.writeObject("not ok");
                            System.out.println("The Server Does not have enough space for the file");
                        }

                        }
                        else if(mes.equalsIgnoreCase("not ok"))
                        {
                            System.out.println("The File is not present in the Client Device");
                        }

                    } else if (receivedinput.equalsIgnoreCase("delete file")) {

                            String filename = (String) in.readObject();
                            File inputfile;

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
                            String choice =(String) in.readObject();
                            if(choice.equalsIgnoreCase("Y"))
                            {
                                int fileid=-1;
                                int reqid=-1;
                                for(int i : Server.fileuploadinfo.keySet())
                                {
                                        if(Server.fileuploadinfo.get(i).equalsIgnoreCase(filename) && Server.fileuploaderinfo.get(i).equalsIgnoreCase(username))
                                        {
                                            fileid=i;
                                        }

                                }

                                Server.fileuploaderinfo.remove(fileid);
                                Server.fileuploadinfo.remove(fileid);
                                Server.write_hashmap(Server.fileuploadinfo,Server.filelistpath);
                                Server.write_hashmap(Server.fileuploaderinfo,Server.uploader_listpath);
                                for(int i : Server.fulfilledrequests.keySet())
                                {

                                        if(Server.fulfilledrequests.get(i).equalsIgnoreCase(filename) && Server.reqfulfiller.get(i).equalsIgnoreCase(username))
                                        {
                                            reqid=i;
                                        }

                                }
                                Server.fulfilledrequests.remove(reqid);
                                Server.reqfulfiller.remove(reqid);
                                Server.write_hashmap(Server.fulfilledrequests,Server.fulfilledreqpath);
                                Server.write_hashmap(Server.reqfulfiller,Server.reqfulfillerpath);

                                inputfile.delete();
                                out.writeObject("The file is Deleted from Server.");
                            } else if (choice.equalsIgnoreCase("N")) {

                            }

                    }

                }
                catch (IOException |  ClassNotFoundException  e)
                {
                    e.printStackTrace();
                    Server.active_users.remove(username);
                    Server.activeusersockets.remove(username);
                    System.out.println(username+" has logged out.");
                    if(receivedinput.equalsIgnoreCase("upload file"))
                    {
                        Server.datamap.remove(fileid);
                    }

                    break;
                }


            }
        }

}
