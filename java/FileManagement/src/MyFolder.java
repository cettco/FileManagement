/*
 * File:MyFolder.java
 * Function: 文件夹信息类
 */
import java.util.Date;
import java.util.LinkedList;


public class MyFolder {

	LinkedList<MyFolder> FolderList;
	LinkedList<MyFile> FileList;
	MyFolder fatherFolder;
	String folderName;
	private String createDate;
	private String changeDate;
	public MyFolder() {
		// TODO Auto-generated constructor stub
		FolderList = new LinkedList<MyFolder>();
		FileList = new LinkedList<MyFile>();
		fatherFolder = null;
		folderName = "root";
		createDate = String.format("%tr", new Date());
		changeDate = " ";
	}
	public MyFolder(MyFolder folder,String name) {
		// TODO Auto-generated constructor stub
		FolderList = new LinkedList<MyFolder>();
		FileList = new LinkedList<MyFile>();
		fatherFolder = folder;
		this.folderName = name;
	}
	public void setDate()
	{
		changeDate = String.format("%tr", new Date());
	}
	public String getChangeDate(){
		return changeDate;
	}
	public String getCreateDate(){
		return createDate;
	}
}
