import java.util.Date;

/*
 * File:MyFile.java
 * Function: �ļ���Ϣ��
 */
public class MyFile {

	String 	fileName;		//�ļ���
	int		fileSize;		//�ļ���С
	String	fileContent;	//�ļ�����
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
