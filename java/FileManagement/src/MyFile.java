import java.util.Date;

/*
 * File:MyFile.java
 * Function: 文件信息类
 */
public class MyFile {

	String 	fileName;		//文件名
	int		fileSize;		//文件大小
	String	fileContent;	//文件内容
	private String createDate;
	private String changeDate;
	MyFile(String fileName)
	{
		this.fileName = fileName;
		createDate = String.format("%tr", new Date());
		changeDate = " ";
	}
	void update()
	{
		fileSize = fileContent.length()*2;
	}
	void setContent(String content)
	{
		fileContent = content;
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
