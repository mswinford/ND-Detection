import java.io.*;
import java.util.*;

public class QueryFile{

static String content =null;
static String topic = null;
static List<String> lines = null;
final static String FILEDIRECTORY = "./data"; 
static String outputfile;

private static String readFile( File file ) throws IOException {
    BufferedReader reader = new BufferedReader( new FileReader (file));
    String         line = null;
    StringBuilder  stringBuilder = new StringBuilder();
    String         ls = System.getProperty("line.separator");

    while( ( line = reader.readLine() ) != null ) {
    	Scanner tokenizer = new Scanner(line);  
     	String word1 = tokenizer.next(); 
     	if(word1.equals("topic")){
     		topic = line;
     	}
     	if(word1.equals("comment")){
        	StringBuffer s = new StringBuffer(line);
 		StringBuffer removed =s.delete(0,10);    
        	line = removed.toString();   
		stringBuilder.append( line );
      		//System.out.println("found");
     		break;
    	 }
     
     
      		//stringBuilder.append( line );
        	//stringBuilder.append( ls );
    }

    while( ( line = reader.readLine() ) != null ) {
    	stringBuilder.append( line );
        //stringBuilder.append( ls );
    }
    
    
    return stringBuilder.toString();
}


public static void listFilesForFolder( File folder) {
	File out = new File(outputfile);
	try{
        	PrintWriter clean = new PrintWriter(out);
         	clean.println("");
        }catch (Exception e){//Catch exception if any
  		System.err.println("Error: " + e.getMessage());
  	}
   	 for (final File fileEntry : folder.listFiles()) {
      		//System.out.println(fileEntry.getName());
      	if(lines.contains(fileEntry.getName())){
        	if (fileEntry.isDirectory()) {
            	listFilesForFolder(fileEntry);
       		 } else {
            		//System.out.println(fileEntry.getName());
          	 String fileName = fileEntry.getName(); 
    		 File parentDir = new File(FILEDIRECTORY); // New file (parent file ..)   
    		 File newFile = new File(parentDir,fileName); //open the file
              try{
      		//System.out.println("here");
   		content=readFile(newFile);
    		}catch(Exception e){
    		//do nothing
   		 }         
               try{
  		// Create file 
 		 PrintWriter write = new PrintWriter(new FileWriter(out, true));

   		write.println("<DOC>");
 		 write.println("<DOCNO>"+fileEntry.getName()+"</DOCNO>");
    		write.println("<TEXT>");
                  write.println(content);
   /* Scanner sc = new Scanner(content);  
     while( sc.hasNext() ) {
      
     String word = sc.next(); 
     if(word==null){
     break;
     }
     //System.out.println(word);
                    write.println(word);
     }*/
                    write.println("</TEXT>");
                      write.println("</DOC>");
  //Close the output stream
 write.close();
  }catch (Exception e){//Catch exception if any
  System.err.println("Error: " + e.getMessage());
  }
        }
    }
    }
}

 static File folder = new File(FILEDIRECTORY);

  
  public static void main(String[] args) {
	outputfile = args[1];
    try{
   Scanner sc = new Scanner(new File(args[0])); 
    lines = new ArrayList<String>();
   while (sc.hasNextLine()) {   
     lines.add(sc.nextLine()); 
   }  
    }catch(Exception e){
    //do nothing
    }
   
  // String[] arr = lines.toArray(new String[0]);
  listFilesForFolder(folder);
  }
    
}
