import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class ContactTest {

	//Tests whether the Contact class accepts valid inputs
	@Test
	void testContactClass(){
		Contact testContact = new Contact("105", "douglas", "Henry", "8011234567", "1010 Nowheresville");
		assertTrue(testContact.getID().equals("105"));
		assertTrue(testContact.getFirstName().equals("douglas"));
		assertTrue(testContact.getLastName().equals("Henry"));
		assertTrue(testContact.getPhone().equals("8011234567"));
		assertTrue(testContact.getAddress().equals("1010 Nowheresville"));

	}
	
	//checking for input validation
	@Test
	void testContactClassInputsTooLong() {
		
		//checks if ID is too long
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			Contact testContact = new Contact("12345678910", "douglas", "Henry", "8011234567", "1010 Nowheresville");
		}); 	
		
		//Checks if first name is too long
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			Contact testContact = new Contact("105", "douglasharringbone", "Henry", "8011234567", "1010 Nowheresville");
		}); 
		
		//checks if last name is too long
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			Contact testContact = new Contact("105", "douglas", "fredericksonson", "8011234567", "1010 Nowheresville");
		}); 
		
		//checks if phone number is too short
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			Contact testContact = new Contact("105", "douglas", "Henry", "801123456", "1010 Nowheresville");
		}); 
		
		//Checks if phone number is too long
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			Contact testContact = new Contact("12345678910", "douglas", "Henry", "801123456710", "1010 Nowheresville");
		}); 
		
		
		//checks if address is too long
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			Contact testContact = new Contact("12345678910", "douglas", "Henry", "8011234567", "1010 Nowheresville Nashville, Ohio");
		}); 
	
	}
	
}
