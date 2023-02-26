import java.util.ArrayList;

public class ContactService {
	
	//Initializes list
	ArrayList<Contact> contactList = new ArrayList<Contact>();
	
	//Method for adding contacts, checks that the ID is unique first
	public void addContact(String ID, String firstName, String lastName, String phone, String address) {
		Contact currentContact;
		for(int i=0; i<contactList.size(); i++) {
			currentContact = contactList.get(i);
			
			if(currentContact.ID == ID) {
				throw new IllegalArgumentException("ID in use");
			}
		}
		currentContact = new Contact(ID, firstName, lastName, phone, address);
		contactList.add(currentContact);
	}
	
	
	//Method for deleting contacts, removes contact at current index and returns
	public void deleteContact(String ID) {
		Contact currentContact;
		for(int i=0; i<contactList.size(); i++){
			currentContact = contactList.get(i);
			if(currentContact.ID == ID) {
				contactList.remove(i);
				return;
			}
		}
		
	}
	
	
	//Methods for updating first name, last name, phone and address. Input validation is handled by setter methods on Contact class 
	
	public void updateFirstName (String ID, String firstName) {
		Contact currentContact;
		for(int i=0; i<contactList.size(); i++){
			currentContact = contactList.get(i);
			if(currentContact.ID == ID) {
				currentContact.setFirstName(firstName);
				return;
			}
		}
	}
	
	public void updateLastName (String ID, String lastName) {
		Contact currentContact;
		for(int i=0; i<contactList.size(); i++){
			currentContact = contactList.get(i);
			if(currentContact.ID == ID) {
				currentContact.setLastName(lastName);
				return;
			}
		}
	}
	
	public void updatePhone (String ID, String phone) {
		Contact currentContact;
		for(int i=0; i<contactList.size(); i++){
			currentContact = contactList.get(i);
			if(currentContact.ID == ID) {
				currentContact.setPhone(phone);
				return;
			}
		}
	}
	
	public void updateAddress (String ID, String address) {
		Contact currentContact;
		for(int i=0; i<contactList.size(); i++){
			currentContact = contactList.get(i);
			if(currentContact.ID == ID) {
				currentContact.setAddress(address);
				return;
			}
		}
	}
	
}
