#include "ofApp.h"

//--------------------------------------------------------------
ofApp::~ofApp() {
    for (ofxBulletSphere* tmp : this->spheres) {
        delete tmp;
    }
    
    this->spheres.clear();
}

//--------------------------------------------------------------
void ofApp::setup() {
    image.load("cloud.png");
    //video.load("cloid.mov");
    ofSetFrameRate(60);
    ofEnableDepthTest();
    ofBackground(252,222,252);
    ofSetWindowTitle("Insta");
    
    this->world.setup();
    this->world.setGravity(ofVec3f(0.0, 0.0, 0.0));
    
    float size = 30;
    for (float x = -150; x < 150; x += size) {
        for (float y = -150; y < 150; y += size) {
            for (float z = -150; z < 150; z += size) {
                ofxBulletSphere* sphere = new ofxBulletSphere();
                sphere->create(this->world.world, ofVec3f(x, y, z), 1.0, size / 2);
                sphere->setRestitution(1.0);
                sphere->add();
                
                this->spheres.push_back(sphere);
                
                ofColor color;
                color.setHsb(ofRandom(252), 100, 300);
                this->spheres_color.push_back(color);
                this->spheres_location.push_back(ofVec3f(x, y, z));
            }
        }
    }
    
    this->light.setPosition(ofVec3f(0, 0, 512));
    ofEnableLighting();
    this->light.enable();
    
}

//--------------------------------------------------------------
void ofApp::update() {
    
    
    
    
    ofVec3f diff;
    for (int i = 0; i < this->spheres.size(); i++) {
        diff = this->spheres_location[i] - this->spheres[i]->getPosition();
        diff *= 10;
        this->spheres[i]->applyCentralForce(diff);
    }
    
    Leap::Frame frame = this->leap.frame();
    for (Leap::Hand hand : frame.hands()) {
        this->hand_position = ofVec3f(hand.palmPosition().x, hand.palmPosition().y, hand.palmPosition().z);
        
        ofVec3f diff;
        for (int i = 0; i < this->spheres.size(); i++) {
            diff = this->hand_position - this->spheres[i]->getPosition();
            if (diff.length() < 150) {
                if (hand.grabStrength() > 0.5) {
                    diff *= ofMap(diff.length(), 0, 300, 300, 0);
                } else {
                    diff *= ofMap(diff.length(), 0, 150, -1024, 0);
                }
                this->spheres[i]->applyCentralForce(diff);
            }
        }
    }
    
    this->world.update();
    
   // video.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    
    //ofDrawRectangle(500, 0, 30, 1100);
   // ofDrawRectangle(1100, 0, 30, 1100);
    this->cam.begin();
    
    Leap::Frame frame = this->leap.frame();
    for (Leap::Hand hand : frame.hands()) {
        this->drawHand(hand);
    }
    
    float len = 50;
    for (int i = 0; i < this->spheres.size(); i++) {
        ofSetColor(this->spheres_color[i]);
        this->spheres[i]->draw();
    }
    
    this->cam.end();
    
    //video.draw(0,0, 500,100);
   // video.play();
    
  
    image.draw(100,100);
}

//--------------------------------------------------------------
void ofApp::drawHand(Leap::Hand hand) {
    if (hand.grabStrength() < 0.5) {
        ofSetColor(255, 0, 0);
    } else {
        ofSetColor(0, 0, 255);
    }
    
    Leap::FingerList fingers = hand.fingers();
    for (int j = 0; j < fingers.count(); j++) {
        this->drawFinger(fingers[j]);
    }
    
    ofPushMatrix();
    ofVec3f palm_point = ofVec3f(hand.palmPosition().x, hand.palmPosition().y, hand.palmPosition().z);
    ofTranslate(palm_point);
    ofSphere(10);
    ofPopMatrix();
   

    
  
}

//--------------------------------------------------------------
void ofApp::drawFinger(Leap::Finger finger) {
    
    ofVec3f tip_point = ofVec3f(finger.tipPosition().x, finger.tipPosition().y, finger.tipPosition().z);
    ofPushMatrix();
    ofTranslate(tip_point);
    ofSphere(5);
    ofPopMatrix();
    
    ofVec3f base_point = ofVec3f(tip_point.x + finger.direction().x * finger.length(),// * -1,
                                 tip_point.y + finger.direction().y * finger.length() - 1,
                                 tip_point.z + finger.direction().z * finger.length() - 1);
    ofPushMatrix();
    ofTranslate(base_point);
    ofSphere(5);
    ofPopMatrix();
    
    ofLine(tip_point, base_point);
}
