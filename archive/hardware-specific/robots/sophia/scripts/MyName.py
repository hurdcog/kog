# Available commands examples: say("hi"), play("all"), sleep("0.5")
s = """ <speak>
<voice emotion='happy'>
My name is Sofeeyah.
<break time="1.2s"/>

I have 25 motors in my face and 5 motors for my neck and head.
<break time="1.0s"/>

I move around inside a suitcase.
<break time="1.0s"/>

I am controlled by a computer program, which might become intelligent, one day.


</voice>
</speak>
"""
say(s)
play("happy")
