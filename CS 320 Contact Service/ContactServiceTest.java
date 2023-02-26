import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class ContactServiceTest {

	@Test
	void testSetMethods() {
		Contact testContact = new Contact("105", "douglas", "Henry", "8011234567", "1010 Nowheresville");
		ContactService testService = new ContactService();
		testService.contactList.add(testContact);
		
		testService.updateFirstName("105", "Brian");
		assertTrue(testContact.getFirstName().equals("Brian"));
		
		testService.updateLastName("105", "Benjie");
		assertTrue(testContact.getLastName().equals("Benjie"));
		
		testService.updatePhone("105", "8017654321");
		assertTrue(testContact.getPhone().equals("8017654321"));
		
		testService.updateAddress("105", "2020 heresville");
		assertTrue(testContact.getAddress().equals("2020 heresville"));
		
	}
	
	@Test
	void testAddContact() {
		Contact testContact = new Contact("105", "douglas", "Henry", "8011234567", "1010 Nowheresville");
		ContactService testService = new ContactService();
		testService.contactList.add(testContact);
		
		testService.addContact("106", "Harry", "Boopbop", "2223334444", "5011 righthere");
		assertTrue(testService.contactList.size() == 2);		
	}
	
	@Test
	void testDeleteContact() {
		Contact testContact = new Contact("105", "douglas", "Henry", "8011234567", "1010 Nowheresville");
		ContactService testService = new ContactService();
		testService.contactList.add(testContact);
		
		testService.deleteContact("105");
		assertTrue(testService.contactList.size() == 0);		
	}
	
	@Test
	void testUniqueID() {
		Contact testContact = new Contact("105", "douglas", "Henry", "8011234567", "1010 Nowheresville");
		ContactService testService = new ContactService();
		testService.contactList.add(testContact);
		
		
		
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			testService.addContact("105", "Harry", "Boopbop", "2223334444", "5011 righthere");
		}); 
	}
	

}
