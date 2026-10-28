"""Foundation-only boundary check of extracted CHECK navigation and actual input handlers."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import subprocess

OUT = Path(__file__).resolve().parent
WT = OUT.parent / 'heartgold-upstream-sync'


def extract(source, anchor):
    start = source.rindex(anchor)
    brace = source.index('{', start)
    depth = 0
    for token in re.finditer(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*.*?\*/|[{}]', source[brace:], re.S):
        if token[0] == '{':
            depth += 1
        elif token[0] == '}':
            depth -= 1
            if not depth:
                return source[start:brace + token.end()]
    raise AssertionError('Unclosed actual source body: ' + anchor)


def main():
    global OUT, WT
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--candidate',type=Path,required=True)
    parser.add_argument('--source-root',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();OUT=args.output.resolve();WT=args.source_root.resolve();candidate_path=args.candidate.resolve()
    OUT.mkdir(parents=True,exist_ok=False)
    candidate = candidate_path.read_text()
    def contract(source):
        keys=set(re.findall(r'report\[@"([^"\n]+)"\]\s*=',source))
        for pattern in (r'NSArray\s*\*keys\s*=\s*@\[(.*?)\];',r'for\s*\(NSString\s*\*key\s+in\s*@\[(.*?)\]\)'):
            for body in re.findall(pattern,source,re.S):keys.update(re.findall(r'@"([^"\n]+)"',body))
        for name,prefix in (('pc_box_check.inc','pcBoxHost'),('trainer_card_uikit_check.inc','trainerCardHost'),('start_menu_uikit_check.inc','startMenuHost'),('pokegear_uikit_check.inc','pokegearHost')):
            keys.update(re.findall(r'@"('+prefix+r'[^"\n]+)"',(WT/'ios/tests'/name).read_text()))
        audio=source.split('static NSArray *OriginalCheckAudioMetricKeys(void) {',1)[1].split('\n}',1)[0]
        keys.difference_update(re.findall(r'@"([^"\n]+)"',audio));keys.add('audioDeviceSuppressed')
        return sorted(keys),sorted(set(re.findall(r'@"(original-[^"\n]+\.(?:png|json))"',source)))
    before=contract((WT/'ios/tests/check_original_app.inc').read_text());after=contract(candidate)
    assert before==after and len(after[0])==295
    main_source = (WT / 'ios/OriginalApp/main.m').read_text()
    helper = extract(candidate, '- (BOOL)checkRequestFieldChild:')
    names = ('- (void)toggleMenu {', '- (void)queueStartMenuInputKeys:', '- (void)touchAt:', '- (void)tapAt:')
    handlers = [extract(main_source, name) for name in names]
    element_method = extract(main_source.split('@implementation OriginalMenuElement', 1)[1], '- (BOOL)accessibilityActivate')
    activation = extract(main_source, 'element.activate=^BOOL') + ';'
    harness = r'''
#import <Foundation/Foundation.h>
#include <CoreGraphics/CGGeometry.h>
#include <dispatch/dispatch.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "native_field_menu_ui.h"
enum {MAP_ROUTE_29=33, UIControlEventTouchUpInside=1};
const CGPoint CGPointZero={0,0};
CGFloat CGRectGetMidX(CGRect r){return r.origin.x+r.size.width/2;}
CGFloat CGRectGetMidY(CGRect r){return r.origin.y+r.size.height/2;}
bool CGRectContainsPoint(CGRect r,CGPoint p){return p.x>=r.origin.x && p.y>=r.origin.y && p.x<r.origin.x+r.size.width && p.y<r.origin.y+r.size.height;}
@class OriginalController;
@interface UIView:NSObject
@property BOOL hidden,userInteractionEnabled;
@property CGRect bounds;
@property(strong) id window,image;
@property(strong) UIView *parent;
@property(strong) NSArray *accessibilityElements;
- (BOOL)isDescendantOfView:(UIView *)view;
@end
@implementation UIView
- (BOOL)isDescendantOfView:(UIView *)view {return self==view || self.parent==view;}
@end
@interface UIButton:UIView
@property BOOL enabled;
@property(copy) NSString *currentTitle,*accessibilityLabel;
@property(weak) OriginalController *target;
@property NSUInteger events;
- (void)sendActionsForControlEvents:(NSUInteger)events;
@end
@interface OriginalMenuElement:NSObject
@property(copy) BOOL (^activate)(void);
@property CGRect accessibilityFrameInContainerSpace;
- (BOOL)accessibilityActivate;
@end
@implementation OriginalMenuElement
__ELEMENT_METHOD__
@end
@interface OriginalController:NSObject {
@public
    NSCondition *_condition;
    NSData *_startMenuRawUIState,*_startMenuUIState,*_displayedStartMenuUIState,*_fieldMenuFrame;
    HGNativeFieldMenuUIState _startMenuChoice;
    BOOL _paused,_stopping,_startMenuChoicePending,_startMenuInputActive,_startMenuInputCooling,_startMenuReleasePending;
    BOOL _startMenuScene,_startMenuBlocksInput,_startMenuChoiceTouch,_fieldMenuOpen,_touchPressed,_automaticMenuEngine;
    BOOL _pokegearScene,_oakScene,_starterScene,_optionsScene,_namingScene,_fieldChoiceScene,_partyScene,_bagScene,_summaryScene;
    BOOL _captureScene,_evolutionScene,_pokedexScene;
    NSString *_failure;
    uint32_t _fieldMapId,_touchEngine,_displayedStartMenuEngine,_startMenuChoiceKeys,_startMenuChoiceX,_startMenuChoiceY,_startMenuChoiceEngine;
    uint32_t _pcBoxOwnerGeneration,_selectedEngine,_keys,_pendingKeys,_submittedKeys,_touchX,_touchY;
    NSUInteger _inputGeneration;
    UIButton *_menuButton;
    UIView *_fieldMenu,*_canvas,*_bagPanel,*_summaryPanel;
    NSArray<OriginalMenuElement *> *_fieldMenuElements;
    uint32_t _fieldMenuIcons;
    NSMutableArray *_checkInputCompletions;
}
@property(strong) UIView *view;
- (BOOL)checkRequestFieldChild:(uint32_t)icon;
- (void)toggleMenu;
- (void)queueStartMenuInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point;
- (void)touchAt:(CGPoint)point pressed:(BOOL)pressed;
- (void)tapAt:(CGPoint)point;
- (BOOL)catchingTutorialBlocksInput;
- (void)resetJoystick;
- (void)queueNamingInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point;
@end
@implementation UIButton
- (void)sendActionsForControlEvents:(NSUInteger)events {self.events++;[self.target toggleMenu];}
@end
static void OriginalSetButtonContent(UIButton *button,id configuration,id image,NSString *title){button.currentTitle=title;}
@implementation OriginalController
- (BOOL)catchingTutorialBlocksInput {return NO;}
- (void)resetJoystick {}
- (void)queueNamingInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point {assert(0);}
__HANDLERS__
__HELPER__
@end
static NSData *packet(HGNativeFieldMenuUIState p){return [NSData dataWithBytes:&p length:sizeof(p)];}
static OriginalController *fixture(BOOL native) {
    OriginalController *c=[OriginalController new];c->_condition=[NSCondition new];c.view=[UIView new];c.view.window=[NSObject new];
    c->_fieldMapId=33;c->_touchEngine=c->_displayedStartMenuEngine=1;
    c->_fieldMenu=[UIView new];c->_fieldMenu.bounds=CGRectMake(0,0,256,192);c->_fieldMenu.hidden=YES;
    c->_fieldMenu.window=c.view.window;c->_fieldMenu.parent=c.view;c->_fieldMenu.userInteractionEnabled=YES;
    c->_menuButton=[UIButton new];c->_menuButton.window=c.view.window;c->_menuButton.parent=c.view;c->_menuButton.target=c;
    c->_menuButton.enabled=c->_menuButton.userInteractionEnabled=YES;c->_checkInputCompletions=[NSMutableArray new];
    c->_canvas=[UIView new];c->_bagPanel=[UIView new];c->_summaryPanel=[UIView new];c->_bagPanel.hidden=c->_summaryPanel.hidden=YES;
    HGNativeFieldMenuUIState p={.active=native,.ownerGeneration=17,.revision=23,.ready=native,.phase=native ? 3 : 0,.viewActive=native,.controlActive=native};
    c->_startMenuRawUIState=c->_startMenuUIState=packet(p);c->_displayedStartMenuUIState=native ? packet(p) : nil;
    c->_startMenuScene=c->_startMenuBlocksInput=native;
    __weak OriginalController *weakSelf=c;NSUInteger i=0;OriginalMenuElement *element=[OriginalMenuElement new];
    element.accessibilityFrameInContainerSpace=CGRectMake(0,0,32,32);
    __ACTIVATION__
    c->_fieldMenuElements=@[element];c->_fieldMenu.accessibilityElements=c->_fieldMenuElements;c->_fieldMenuIcons=2;
    return c;
}
int main(void){@autoreleasepool{
    OriginalController *c=fixture(YES);
    BOOL oldPath=!c->_fieldMenu.hidden && c->_fieldMenu.userInteractionEnabled && [c->_fieldMenuElements[0] accessibilityActivate];
    assert(!oldPath && !c->_menuButton.events);
    assert(![c checkRequestFieldChild:2]);
    assert(c->_menuButton.events==1 && c->_startMenuChoicePending && c->_startMenuChoiceKeys==2);
    assert(c->_startMenuChoice.ownerGeneration==17 && c->_startMenuChoice.revision==23 && !c->_fieldMenuOpen && !c->_touchPressed);
    HGNativeFieldMenuUIState retired={0};c->_startMenuRawUIState=c->_startMenuUIState=packet(retired);c->_displayedStartMenuUIState=nil;
    c->_startMenuScene=c->_startMenuBlocksInput=c->_startMenuChoicePending=c->_startMenuInputActive=c->_startMenuInputCooling=c->_startMenuReleasePending=NO;
    assert(![c checkRequestFieldChild:2] && c->_fieldMenuOpen && c->_menuButton.events==2);
    c->_fieldMenu.hidden=NO;assert([c checkRequestFieldChild:2] && c->_touchPressed && c->_checkInputCompletions.count==1);
    c=fixture(YES);c->_fieldMenu.hidden=NO;c->_startMenuInputCooling=YES;
    assert([c->_fieldMenuElements[0] accessibilityActivate] && !c->_startMenuChoicePending && !c->_touchPressed);
    assert(![c checkRequestFieldChild:2] && !c->_menuButton.events);
    for(unsigned variant=0;variant<6;variant++){
        c=fixture(YES);HGNativeFieldMenuUIState p=*(const HGNativeFieldMenuUIState *)c->_startMenuUIState.bytes;
        if(variant==0){p.revision++;c->_startMenuUIState=c->_startMenuRawUIState=packet(p);}
        else if(variant==1)c->_startMenuInputCooling=YES;
        else if(variant==2){p.ownerGeneration++;c->_startMenuRawUIState=packet(p);}
        else if(variant==3)c->_touchEngine=0;
        else if(variant==4)c->_startMenuChoicePending=YES;
        else {p.childActive=1;c->_startMenuUIState=c->_startMenuRawUIState=c->_displayedStartMenuUIState=packet(p);}
        assert(![c checkRequestFieldChild:2] && !c->_menuButton.events && !c->_touchPressed);
    }
    c=fixture(NO);assert(![c checkRequestFieldChild:2] && c->_fieldMenuOpen && c->_menuButton.events==1);
    c->_fieldMenu.hidden=NO;assert([c checkRequestFieldChild:2]);
    assert(c->_touchPressed && c->_touchX==16 && c->_touchY==16 && c->_checkInputCompletions.count==1);
    c=fixture(NO);c->_fieldMenuOpen=YES;c->_fieldMenu.hidden=NO;c->_partyScene=YES;
    assert([c->_fieldMenuElements[0] accessibilityActivate] && !c->_touchPressed);
    assert(![c checkRequestFieldChild:2]);
    c=fixture(NO);c->_fieldMenuOpen=YES;c->_fieldMenu.hidden=NO;c->_startMenuRawUIState=[NSData data];
    assert(![c checkRequestFieldChild:2] && !c->_touchPressed && !c->_menuButton.events);
    puts("PASS: extracted CHECK helper and actual native/legacy handlers admit only fresh owned navigation");
}}
'''
    harness = harness.replace('__ELEMENT_METHOD__', element_method).replace('__HANDLERS__', '\n'.join(handlers)).replace('__HELPER__', helper).replace('__ACTIVATION__', activation)
    source = OUT / 'navigation-boundary.m'; source.write_text(harness)
    binary = OUT / 'navigation-boundary'
    argv = ['xcrun', 'clang', '-std=c11', '-O1', '-g', '-fobjc-arc', '-fblocks', '-Werror', '-DHEARTGOLD_ORIGINAL_APP_CHECK',
            '-fsanitize=address,undefined', '-fno-sanitize-recover=all', '-iquote', str(WT / 'include'), str(source), '-framework', 'Foundation', '-o', str(binary)]
    commands = []
    for label, command in [('compile', argv), ('symbols', ['nm', '-u', str(binary)]), ('dependencies', ['otool', '-L', str(binary)])]:
        r = subprocess.run(command, capture_output=True, text=True, timeout=30)
        (OUT / (label + '.log')).write_text(r.stdout + r.stderr)
        commands.append(dict(label=label, argv=command, returnCode=r.returncode))
        assert r.returncode == 0, label + ' failed; inspect ' + str(OUT / (label + '.log'))
        if label != 'compile':
            assert not re.search(r'AVFAudio|AVFoundation|AVAudio|AudioToolbox|CoreAudio|UIKit', r.stdout)
    r = subprocess.run([str(binary)], capture_output=True, text=True, timeout=13)
    (OUT / 'runtime.log').write_text(r.stdout + r.stderr);assert r.returncode == 0, 'Boundary runtime failed'
    result = dict(passed=True, actualHelperSha256=hashlib.sha256(helper.encode()).hexdigest(),
                  actualHandlerSha256={name:hashlib.sha256(body.encode()).hexdigest() for name,body in zip(names,handlers)},
                  actualElementActivationSha256=hashlib.sha256(activation.encode()).hexdigest(), candidateSha256=hashlib.sha256(candidate_path.read_bytes()).hexdigest(),
                  sourceRoot=str(WT),inputCandidate=str(candidate_path),sameBooleanContract=True,booleanKeys=len(after[0]),metadataKeys=2,
                  jsonFiles=sum(n.endswith('.json') for n in after[1]),pngFiles=sum(n.endswith('.png') for n in after[1]),
                  commands=commands,runtimeReturnCode=r.returncode,allChildrenWaited=True,frameworks=['Foundation'],
                  legacyOnlyPathDeferredUnderNativeOwnership=True,freshNativeCloseAdmitted=True,staleBusyWrongOwnerEngineRejected=True,
                  legitimateLegacyUsesActualElementTapTouch=True,falseActivationDoesNotAdvance=True,invalidOwnerDoesNotFallBack=True,
                  sameControllerRetireOpenTouchAdmissionChain=True,visibleNativeLegacyYesWithoutAdmissionRejected=True,
                  appRuntimeExecuted=False,simulatorOperations=False,audioDeviceOperations=False)
    (OUT / 'boundary-report.json').write_text(json.dumps(result,indent=2,sort_keys=True)+'\n');print(json.dumps(result,sort_keys=True))


if __name__ == '__main__':main()
